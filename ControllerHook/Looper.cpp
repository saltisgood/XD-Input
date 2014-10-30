#include "Looper.h"

#include <Windows.h>

void Hook::loop(Hook::Hid::HidControllerDevice &realD, Hook::Scp::VirtualDevice &vjd)
{
	for (;;)
	{
		Sleep(1);

		realD.pullData();
		vjd.feed(realD.parseData());

		if (GetAsyncKeyState(VK_END))
		{
			break;
		}
	}
}