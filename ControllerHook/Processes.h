#ifndef __CTRLHOOK_PROCESS_H_
#define __CTRLHOOK_PROCESS_H_

#include <forward_list>
#include <string>

namespace Hook
{
	std::forward_list<std::wstring>::const_iterator checkProcessActive(const std::forward_list<std::wstring>::const_iterator &processListStart,
		const std::forward_list<std::wstring>::const_iterator &processListEnd);

	bool checkProcessAlive(const std::wstring &processName);
}

#endif