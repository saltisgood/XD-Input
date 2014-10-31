// ControllerHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Looper.h"
#include "Util.h"
#include "ScpDevice.h"
#include "HID_Util.h"
#include "Util.h"

#include <conio.h>
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	auto list = Hook::Hid::enumerateHIDDevicesAlt();

	if (list.empty())
	{
		Hook::errorMessage("No HID devices found!");
		return EXIT_FAILURE;
	}

	auto devIt(list.cbegin());

	while (devIt != list.cend())
	{
		std::wcout << "Found HID device: " << devIt->description << ". Press y to select, anything else to skip: ";

		int c(_getch());

		printf_s("%c\n", c);

		if (c == 'y')
		{
			break;
		}

		++devIt;
	}

	if (devIt == list.cend())
	{
		std::cout << "\nAll HID devices finished, none selected. Check the connection of your device and restart the program." << std::endl;
		Hook::pause();
		return EXIT_SUCCESS;
	}

	Hook::Hid::HidControllerDevice dev(*devIt);
	dev.openDevice(true);

	Hook::Scp::BusDevice bus;
	
	std::cout << "Connecting to ScpServer... ";
	if (!bus.open() || !bus.start())
	{
		Hook::errorMessage("Failed to open stream to ScpServer! Have you installed the virtual driver?");
		Hook::pause();
		return EXIT_FAILURE;
	}

	std::cout << "Connection successful!\n" << std::endl;

	Hook::loop(dev, bus.retrieveDevice());

	return EXIT_SUCCESS;
}

