#include "Processes.h"

#include <Windows.h>
#include <Psapi.h>

std::forward_list<std::wstring>::const_iterator Hook::checkProcessActive(const std::forward_list<std::wstring>::const_iterator &start,
	const std::forward_list<std::wstring>::const_iterator &end)
{
	DWORD processes[1024];
	DWORD needed;

	if (!EnumProcesses(processes, sizeof(processes), &needed))
	{
		return end;
	}

	DWORD procCount(needed / sizeof(DWORD));

	TCHAR procName[MAX_PATH] = TEXT("<unknown>");

	for (unsigned int i = 0; i < procCount; ++i)
	{
		if (processes[i])
		{
			HANDLE hProc(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]));

			if (hProc != NULL)
			{
				HMODULE hMod;
				
				if (EnumProcessModules(hProc, &hMod, sizeof(HMODULE), &needed))
				{
					GetModuleBaseName(hProc, hMod, procName, sizeof(procName) / sizeof(TCHAR));

					std::wstring wstrName(procName);
					
					for (auto iter = start; iter != end; ++iter)
					{
						if (!wstrName.compare(*iter))
						{
							return iter;
						}
					}
				}
			}

			CloseHandle(hProc);
		}
	}

	return end;
}

bool Hook::checkProcessAlive(const std::wstring &proc)
{
	std::forward_list<std::wstring> list;
	list.push_front(proc);

	return checkProcessActive(list.cbegin(), list.cend()) != list.cend();
}