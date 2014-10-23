#include "Looper.h"

#include "HookDXUtil.h"

#include <Windows.h>
#include <dinput.h>
#include <wbemidl.h>
#include <OleAuto.h>

Hook::Looper::Looper(GUID gui) :
	mGuid(gui)
{}

bool Hook::Looper::loop()
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
	for (;;)
	{
		Sleep(100);

		hr = controller->Poll();

		if (FAILED(hr))
		{
			// Oh-a-noes!
		}

		hr = controller->GetDeviceState(sizeof(DIJOYSTATE), &state);

		if (FAILED(hr))
		{
			// Oh-a-noes-2!
		}
	}

	controller->Unacquire();
	return true;
}