// ControllerHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Looper.h"
#include "Util.h"
#include "ScpDevice.h"
#include "HID_Util.h"

#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	auto list = Hook::Hid::enumerateHIDDevicesAlt();

	Hook::Hid::HidControllerDevice dev(list.front());
	dev.openDevice(true);

	if (list.empty())
	{
		std::cout << "No HID devices found!" << std::endl;
	}

	Hook::Scp::BusDevice bus;
	
	std::cout << "Connecting to ScpServer..." << std::endl;
	if (!bus.open() || !bus.start())
	{
		Hook::errorMessage("Failed to open stream to ScpServer!");
		return EXIT_FAILURE;
	}

	std::cout << "Connection successful. Starting controller translation.\n\nPress END to cancel..." << std::endl;

	Hook::loop(dev, bus.retrieveDevice());

	return EXIT_SUCCESS;
}

