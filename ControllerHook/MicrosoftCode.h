#ifndef __CTRLHOOK_MIC_CODE_H
#define __CTRLHOOK_MIC_CODE_H

#include <forward_list>

#include <guiddef.h>

namespace Hook
{
	bool genControllerList(std::forward_list<GUID>& xInputDevices, std::forward_list<GUID>& dInputDevices);

	bool isXInputDevice(const GUID *guid);
}

#endif