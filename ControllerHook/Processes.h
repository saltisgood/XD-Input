#ifndef __CTRLHOOK_PROCESS_H_
#define __CTRLHOOK_PROCESS_H_

#include <forward_list>
#include <string>

namespace Hook
{
	// Check whether any processes in a list are currently active. Returns an interator to the process name if it's
	// active. If none are active, returns processListEnd.
	std::forward_list<std::wstring>::const_iterator checkProcessActive(const std::forward_list<std::wstring>::const_iterator &processListStart,
		const std::forward_list<std::wstring>::const_iterator &processListEnd);

	// Check whether a process with the given name is alive.
	bool checkProcessAlive(const std::wstring &processName);
}

#endif