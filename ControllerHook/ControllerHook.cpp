// ControllerHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "HookDXUtil.h"
#include "Looper.h"
#include "MicrosoftCode.h"
#include "Util.h"
#include "ScpDevice.h"

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

	Hook::Scp::BusDevice bus;
	
	if (!bus.open() || !bus.start())
	{
		Hook::errorMessage("Failed to open stream to ScpServer!");
		return EXIT_FAILURE;
	}

	Hook::Looper looper(dInputs.front());
	looper.loop(bus.retrieveDevice());

	return EXIT_SUCCESS;
}

