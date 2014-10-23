// ControllerHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "HookDXUtil.h"
#include "Looper.h"
#include "MicrosoftCode.h"
#include "Util.h"

/*
bool checkControllerConnected(DWORD controllerNum)
{
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	return XInputGetState(controllerNum, &state) == ERROR_SUCCESS;
} */

int _tmain(int argc, _TCHAR* argv[])
{
	if (!Hook::DInput8_Util::create())
	{
		Hook::errorMessage("Failed to created DInput8_Util!");
		return EXIT_FAILURE;
	}

	std::forward_list<GUID> xInputs;
	std::forward_list<GUID> dInputs;

	if (!Hook::genControllerList(xInputs, dInputs))
	{
		Hook::errorMessage("Failed to generate list of controllers!");
		return EXIT_FAILURE;
	}

	if (dInputs.empty())
	{
		Hook::errorMessage("No DirectInput controllers found!");
		return EXIT_SUCCESS;
	}

	Hook::Looper looper(dInputs.front());
	looper.loop();

	return EXIT_SUCCESS;
}

