#ifndef __CTRLHOOK_PROCESS_H_
#define __CTRLHOOK_PROCESS_H_

#include "Macros.h"

namespace Hook
{
	// Check whether any processes in a list are currently active. Returns an interator to the process name if it's
	// active. If none are active, returns processListEnd.
	std::forward_list<HOOK_TCHARSTR>::const_iterator checkProcessActive(const std::forward_list<HOOK_TCHARSTR>::const_iterator &processListStart,
		const std::forward_list<HOOK_TCHARSTR>::const_iterator &processListEnd);

	// Check whether a process with the given name is alive.
	bool checkProcessAlive(const HOOK_TCHARSTR &processName);
}

#endif