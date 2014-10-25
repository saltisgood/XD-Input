#include "Looper.h"

#include "HookDXUtil.h"

#include <Windows.h>
#include <dinput.h>
#include <wbemidl.h>
#include <OleAuto.h>

void translateState(DIJOYSTATE& diState, Hook::ControllerState& myState)
{
	myState.button_primary.button_y = diState.rgbButtons[0] ? 1 : 0;
	myState.button_primary.button_b = diState.rgbButtons[1] ? 1 : 0;
	myState.button_primary.button_a = diState.rgbButtons[2] ? 1 : 0;
	myState.button_primary.button_x = diState.rgbButtons[3] ? 1 : 0;

	myState.button_primary.left_shoulder = diState.rgbButtons[4] ? 1 : 0;
	myState.button_primary.right_shoulder = diState.rgbButtons[5] ? 1 : 0;

	myState.button_primary.guide = 0;
	
	myState.left_trigger = diState.rgbButtons[6] ? UCHAR_MAX : 0;
	myState.right_trigger = diState.rgbButtons[7] ? UCHAR_MAX : 0;

	myState.button_secondary.back = diState.rgbButtons[8] ? 1 : 0;
	myState.button_secondary.start = diState.rgbButtons[9] ? 1 : 0;
	myState.button_secondary.left_stick = diState.rgbButtons[10] ? 1 : 0;
	myState.button_secondary.right_stick = diState.rgbButtons[11] ? 1 : 0;

	myState.left_stick_x = static_cast<int16_t>(diState.lX);
	myState.left_stick_y = static_cast<int16_t>(diState.lY);
	
	myState.right_stick_x = static_cast<int16_t>(diState.lZ);
	myState.right_stick_y = static_cast<int16_t>(diState.lRz);

	myState.button_secondary.hat_up = 0;
	myState.button_secondary.hat_down = 0;
	myState.button_secondary.hat_left = 0;
	myState.button_secondary.hat_right = 0;

	DWORD povHat = diState.rgdwPOV[0];

	if (povHat > 0 && povHat < (180 * DI_DEGREES))
	{
		myState.button_secondary.hat_right = 1;
	}

	if (povHat > (90 * DI_DEGREES) && povHat < (270 * DI_DEGREES))
	{
		myState.button_secondary.hat_down = 1;
	}

	if (povHat > (180 * DI_DEGREES) && povHat < (360 * DI_DEGREES))
	{
		myState.button_secondary.hat_left = 1;
	}

	if ((povHat >= 0 && povHat < (90 * DI_DEGREES)) || (povHat > (270 * DI_DEGREES) && povHat < (360 * DI_DEGREES)))
	{
		myState.button_secondary.hat_up = 1;
	}
}

Hook::Looper::Looper(GUID gui) :
	mGuid(gui)
{}

bool Hook::Looper::loop(Hook::Scp::VirtualDevice &vjd)
{
	LPDIRECTINPUTDEVICE8 controller;

	HRESULT hr = HOOKDIN8_GET->CreateDevice(mGuid, &controller, NULL);

	if (FAILED(hr))
	{
		return false;
	}

	controller->SetDataFormat(&c_dfDIJoystick);
	hr = controller->Acquire();

	if (FAILED(hr))
	{
		return false;
	}

	DIJOYSTATE state;
	Hook::ControllerState myState;
	initialiseState(myState, vjd.deviceNum);

	for (;;)
	{
		Sleep(20);

		hr = controller->Poll();

		if (FAILED(hr))
		{
			// Oh-a-noes!
			continue;
		}

		hr = controller->GetDeviceState(sizeof(DIJOYSTATE), &state);

		if (FAILED(hr))
		{
			// Oh-a-noes-2!
			continue;
		}

		translateState(state, myState);

		vjd.feed(myState);

		if (GetAsyncKeyState(VK_END))
		{
			break;
		}
	}

	controller->Unacquire();
	return true;
}