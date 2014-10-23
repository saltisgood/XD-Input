// ControllerHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <Xinput.h>
#include <wbemidl.h>
#include <OleAuto.h>
#include <dinput.h>

#include <iostream>
#include <vector>

#define SAFE_RELEASE(x) { if (x) { x->Release(); x = NULL; }}
#define SHIT(msg) std::cout << "Aw shit: " msg << std::endl

std::vector<GUID> xinputDevices;
std::vector<GUID> dinputDevices;

//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains 
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput 
//-----------------------------------------------------------------------------
BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
	IWbemLocator*           pIWbemLocator = NULL;
	IEnumWbemClassObject*   pEnumDevices = NULL;
	IWbemClassObject*       pDevices[20] = { 0 };
	IWbemServices*          pIWbemServices = NULL;
	BSTR                    bstrNamespace = NULL;
	BSTR                    bstrDeviceID = NULL;
	BSTR                    bstrClassName = NULL;
	DWORD                   uReturned = 0;
	bool                    bIsXinputDevice = false;
	UINT                    iDevice = 0;
	VARIANT                 var;
	HRESULT                 hr;

	// CoInit if needed
	hr = CoInitialize(NULL);
	bool bCleanupCOM = SUCCEEDED(hr);

	// Create WMI
	hr = CoCreateInstance(__uuidof(WbemLocator),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWbemLocator),
		(LPVOID*)&pIWbemLocator);
	if (FAILED(hr) || pIWbemLocator == NULL)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if (bstrNamespace == NULL) goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");   if (bstrClassName == NULL) goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");          if (bstrDeviceID == NULL)  goto LCleanup;

	// Connect to WMI 
	hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
		0L, NULL, NULL, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == NULL)
		goto LCleanup;

	// Switch security level to IMPERSONATE. 
	CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == NULL)
		goto LCleanup;

	// Loop over all devices
	for (;;)
	{
		// Get 20 at a time
		hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
		if (FAILED(hr))
			goto LCleanup;
		if (uReturned == 0)
			break;

		for (iDevice = 0; iDevice<uReturned; iDevice++)
		{
			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
			{
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
				// This information can not be found from DirectInput 
				if (wcsstr(var.bstrVal, L"IG_"))
				{
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
						dwVid = 0;
					WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
						dwPid = 0;

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if (dwVidPid == pGuidProductFromDirectInput->Data1)
					{
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			SAFE_RELEASE(pDevices[iDevice]);
		}
	}

LCleanup:
	if (bstrNamespace)
		SysFreeString(bstrNamespace);
	if (bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if (bstrClassName)
		SysFreeString(bstrClassName);
	for (iDevice = 0; iDevice<20; iDevice++)
		SAFE_RELEASE(pDevices[iDevice]);
	SAFE_RELEASE(pEnumDevices);
	
	SAFE_RELEASE(pIWbemLocator);
	SAFE_RELEASE(pIWbemServices);

	if (bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}


//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance,
	VOID* pContext)
{
	HRESULT hr;

	if (IsXInputDevice(&pdidInstance->guidProduct))
	{
		xinputDevices.push_back(pdidInstance->guidProduct);
		return DIENUM_CONTINUE;
	}

	// Device is verified not XInput, so add it to the list of DInput devices
	dinputDevices.push_back(pdidInstance->guidProduct);
	return DIENUM_CONTINUE;
}

bool checkControllerConnected(DWORD controllerNum)
{
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	return XInputGetState(controllerNum, &state) == ERROR_SUCCESS;
}

LPDIRECTINPUT8 lpdi;
bool checkDInputControllerConnected()
{
	HRESULT result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *)&lpdi, NULL);

	if (FAILED(result))
	{
		return false;
	}

	if (lpdi->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, 0, DIEDFL_ATTACHEDONLY) == DI_OK)
	{
		std::cout << "Enum Devices went okay!" << std::endl << dinputDevices.size() << " DInput Devices, " << xinputDevices.size() <<
			" XInput Devices" << std::endl;
		return true;
	}
	else
	{
		SHIT("Enum Devices failed!");
		return false;
	}
}

void waitOnController()
{
	if (dinputDevices.size() != 0)
	{
		LPDIRECTINPUTDEVICE8 lpdiControl;
#define EXIT_WAIT lpdiControl->Unacquire();
		HRESULT hr = lpdi->CreateDevice(dinputDevices[0], &lpdiControl, NULL);

		if (FAILED(hr))
		{
			SHIT("Create Device failed!");
			return;
		}

		lpdiControl->SetDataFormat(&c_dfDIJoystick);
		hr = lpdiControl->Acquire();

		if (FAILED(hr))
		{
			SHIT("Acquire failed!");
			EXIT_WAIT
			return;
		}

		DIJOYSTATE state;
		for (;;)
		{
			Sleep(100);

			hr = lpdiControl->Poll();

			if (FAILED(hr))
			{
				SHIT("Poll failed!");
			}

			hr = lpdiControl->GetDeviceState(sizeof(DIJOYSTATE), &state);

			if (FAILED(hr))
			{
				SHIT("Poll 2 failed!");
			}

			if (state.rgbButtons[0])
			{
				std::cout << "Y ";
			}
			else if (state.rgbButtons[1])
			{
				std::cout << "B ";
			}
			else if (state.rgbButtons[2])
			{
				std::cout << "A ";
			}
			else if (state.rgbButtons[3])
			{
				std::cout << "X ";
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	for (DWORD i = 0; i < 4; ++i)
	{
		std::cout << "Controller " << i << ", connected: " << (checkControllerConnected(i) ? "true" : "false") << std::endl;
	}

	if (checkDInputControllerConnected())
		waitOnController();

	return 0;
}

