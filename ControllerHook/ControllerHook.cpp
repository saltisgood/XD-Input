// ControllerHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "HookDXUtil.h"
#include "Looper.h"
#include "MicrosoftCode.h"
#include "Util.h"
#include "ScpDevice.h"

#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	if (!Hook::DInput8_Util::create())
	{
		Hook::errorMessage("Failed to created DInput8_Util!");
		return EXIT_FAILURE;
	}

	std::forward_list<GUID> xInputs;
	std::forward_list<GUID> dInputs;

	std::cout << "Searching for DirectInput controllers..." << std::endl;

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

	GUID guid = dInputs.front();

	std::cout << std::hex << std::noshowbase << "Found DirectInput controller with GUID: ";

	OLECHAR *bstrGuid;
	StringFromCLSID(guid, &bstrGuid);

	std::wcout << bstrGuid << std::endl;

	CoTaskMemFree(bstrGuid);

	Hook::Scp::BusDevice bus;
	
	std::cout << "Connecting to ScpServer..." << std::endl;
	if (!bus.open() || !bus.start())
	{
		Hook::errorMessage("Failed to open stream to ScpServer!");
		return EXIT_FAILURE;
	}

	std::cout << "Connection successful. Starting controller translation.\n\nPress END to cancel..." << std::endl;

	Hook::Looper looper(guid);
	looper.loop(bus.retrieveDevice());

	return EXIT_SUCCESS;
}

