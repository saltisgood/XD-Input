// ControllerHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Looper.h"
#include "Util.h"
#include "ScpDevice.h"
#include "HID_Util.h"
#include "Util.h"
#include "Config.h"

#include <conio.h>
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	auto list = Hook::Hid::enumerateHIDDevices();

	if (list.empty())
	{
		Hook::errorMessage("No HID devices found!");
		goto failure;
	}
	else
	{
		auto devIt(list.cbegin());

		if (Hook::Config::create() && Hook::Config::get()->hasFavouriteDevice())
		{
			auto vendor(Hook::Config::get()->getFavouriteVendorID());
			auto product(Hook::Config::get()->getFavouriteProductID());

			while (devIt != list.cend())
			{
				Hook::Hid::HidControllerDevice dev(*devIt);

				if (dev.getAttributes().VendorID == vendor && dev.getAttributes().ProductID == product)
				{
					goto endsearch;
				}

				++devIt;
			}

			std::cout << "Favourite device not connected! Listing connected devices:" << std::endl;

			devIt = list.cbegin();
		}

		while (devIt != list.cend())
		{
			std::wcout << "Found HID device: " << devIt->description << ". Press y to select, anything else to skip: ";

			auto c(Hook::getChar());

			if (c == 'y')
			{
				break;
			}

			++devIt;
		}

	endsearch:
		if (devIt == list.cend())
		{
			std::cout << "\nAll HID devices finished, none selected. Check the connection of your device and restart the program." << std::endl;
		}
		else
		{
			Hook::Hid::HidControllerDevice dev(*devIt);
			dev.openDevice(true);

			Hook::Scp::BusDevice bus;

			std::cout << "Connecting to ScpServer... ";
			if (!bus.open() || !bus.start())
			{
				Hook::errorMessage("Failed to open stream to ScpServer! Have you installed the virtual driver?");
				goto failure;
			}

			std::cout << "Connection successful!\n" << std::endl;

			Hook::loop(dev, bus.retrieveDevice());

			if (!Hook::Config::get()->hasFavouriteDevice() || Hook::Config::get()->getFavouriteVendorID() != dev.getAttributes().VendorID ||
				Hook::Config::get()->getFavouriteProductID() != dev.getAttributes().ProductID)
			{
				std::cout << "\nWould you like to save this device as your favourite for the future? y for yes, n for no: ";
				
				int c;
				do {
					c = _getch();
				} while (c != 'y' && c != 'n');
				printf_s("%c\n", c);

				if (c == 'y')
				{
					Hook::Config::get()->setFavouriteVendorID(dev.getAttributes().VendorID);
					Hook::Config::get()->setFavouriteProductID(dev.getAttributes().ProductID);
					std::cout << "Settings updated." << std::endl;
				}
			}
		}
	}

	Hook::Config::destroy();
	return EXIT_SUCCESS;

failure:
	Hook::pause();
	Hook::Config::destroy();
	return EXIT_FAILURE;
}