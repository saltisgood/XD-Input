#ifndef __CTRLHOOK_LOOPER_H_
#define __CTRLHOOK_LOOPER_H_

#include "HID_Util.h"
#include "ScpDevice.h"

namespace Hook
{
	void loop(Hid::HidControllerDevice &realDevice, Scp::VirtualDevice &virtualDevice);
}

#endif