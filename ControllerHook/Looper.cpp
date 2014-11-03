#include "Looper.h"

#include "Util.h"
#include "Config.h"
#include "Processes.h"

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <atomic>

#pragma region Helper

// Helper function for getting the value of the ControllerState bitfields that represent
// the state of the button enum
inline uint8_t getButtonValue(Hook::ControllerState &state, Hook::Buttons butt)
{
#define HOOK_BUTT_ENUM(name) case Hook::Buttons::BUTTON_##name:

	switch (butt)
	{
		HOOK_BUTT_ENUM(Y)
			return state.button_primary.button_y;
		HOOK_BUTT_ENUM(B)
			return state.button_primary.button_b;
		HOOK_BUTT_ENUM(A)
			return state.button_primary.button_a;
		HOOK_BUTT_ENUM(X)
			return state.button_primary.button_x;
		HOOK_BUTT_ENUM(LB)
			return state.button_primary.left_shoulder;
		HOOK_BUTT_ENUM(RB)
			return state.button_primary.right_shoulder;
		HOOK_BUTT_ENUM(BACK)
			return state.button_secondary.back;
		HOOK_BUTT_ENUM(START)
			return state.button_secondary.start;
		HOOK_BUTT_ENUM(LS)
			return state.button_secondary.left_stick;
		HOOK_BUTT_ENUM(RS)
			return state.button_secondary.right_stick;
		HOOK_BUTT_ENUM(LT)
			return state.left_trigger;
		HOOK_BUTT_ENUM(RT)
			return state.right_trigger;
	}

	return 0;
}

// Helper function for setting the ControllerState bitfields with a value according to
// their button enum
inline void setButtonValue(Hook::ControllerState &state, Hook::Buttons butt, uint8_t val)
{
	switch (butt)
	{
		HOOK_BUTT_ENUM(Y)
			state.button_primary.button_y = val;
			break;
		HOOK_BUTT_ENUM(B)
			state.button_primary.button_b = val;
			break;
		HOOK_BUTT_ENUM(A)
			state.button_primary.button_a = val;
			break;
		HOOK_BUTT_ENUM(X)
			state.button_primary.button_x = val;
			break;
		HOOK_BUTT_ENUM(LB)
			state.button_primary.left_shoulder = val;
			break;
		HOOK_BUTT_ENUM(RB)
			state.button_primary.right_shoulder = val;
			break;
		HOOK_BUTT_ENUM(BACK)
			state.button_secondary.back = val;
			break;
		HOOK_BUTT_ENUM(START)
			state.button_secondary.start = val;
			break;
		HOOK_BUTT_ENUM(LS)
			state.button_secondary.left_stick = val;
			break;
		HOOK_BUTT_ENUM(RS)
			state.button_secondary.right_stick = val;
			break;
		HOOK_BUTT_ENUM(LT)
			state.left_trigger = val ? UCHAR_MAX : 0;
			break;
		HOOK_BUTT_ENUM(RT)
			state.right_trigger = val ? UCHAR_MAX : 0;
			break;
	}

#undef HOOK_BUTT_ENUM
}

#pragma endregion

#pragma region Remapper

const std::string Hook::Remapper::DEFAULT_PATH = "keys.config";

Hook::Remapper Hook::Remapper::create(const std::wstring &filePath)
{
	return create(wideToByteString(filePath));
}

Hook::Remapper Hook::Remapper::create(const std::string& path)
{
	Remapper rtn;

	std::string filePath(path);

	// If the path doesn't end in .config, like the case of a process name, append it.
	if (filePath.find(".config") == std::string::npos)
	{
		filePath += ".config";
	}

	std::ifstream file(filePath);

	std::cout << "Remapping buttons... ";

	if (!file)
	{
		std::cout << filePath << " config file not found! Using default values." << std::endl;

		// File not found or available, print the default template.
		printTemplate(filePath);
		rtn.mIsDefault = true;
		return rtn;
	}

	std::cout << filePath << " config file found! Reading values.\n";

	char buff[256];

	file.getline(buff, sizeof(buff));

	while (file)
	{
		std::string line(buff);

		if (!line.empty())
		{
			auto pos(line.find_first_of('='));

			if (pos != std::string::npos)
			{
				auto key(line.substr(0, pos));
				auto val(line.substr(pos + 1, std::string::npos));

				auto kButt(parseButtonString(key));
				auto vButt(parseButtonString(val));

				if (kButt == Buttons::INVALID || vButt == Buttons::INVALID)
				{
					const static std::string err("Invalid Button Value: ");
					errorMessage(err + line);
				}
				else if (kButt != vButt)
				{
					rtn.mMappings.insert(std::pair<Buttons, Buttons>(kButt, vButt));
					std::cout << "Enabled remapping: " << key << " -> " << val << std::endl;
				}
			}
		}

		file.getline(buff, sizeof(buff));
	}

	if (rtn.mMappings.empty())
	{
		std::cout << "No remapped keys found. Ignoring.\n" << std::endl;
		rtn.mIsDefault = true;
	}
	else
	{
		std::cout << "Finished button remapping setup.\n" << std::endl;
	}

	return rtn;
}

bool Hook::Remapper::reread(const std::wstring &path, Hook::Remapper &map)
{
	std::wstring filePath(path);

	// As above, if path doesn't end in .config, append it.
	if (filePath.find(L".config") == std::string::npos)
	{
		filePath += L".config";
	}

	std::ifstream file(filePath);
	// Check whether file is available
	if (file.is_open())
	{
		file.close();
		// If so, close it and recreate the object
		map = create(filePath);
		return true;
	}

	return false;
}

void Hook::Remapper::printTemplate(const std::string& path)
{
	std::ofstream file(path);

	if (!file)
	{
		return;
	}

	for (int i = 0; i < NUM_BUTTONS; ++i)
	{
		const std::string &str(getButtonString(static_cast<Buttons>(i)));

		file << str << "=" << str << "\n";
	}
}

Hook::Remapper::Remapper() :
	mIsDefault(false),
	mMappings()
{}

void Hook::Remapper::remapPriv(Hook::ControllerState &state) const
{
	ControllerState swapState(state);

	for (auto& iter : mMappings)
	{
		setButtonValue(state, iter.first, getButtonValue(swapState, iter.second));
	}
}

#pragma endregion

#pragma region Looping

// Wait until a process in the given list has begun. The function checks whether any are running and if none are,
// sleeps for half a second before trying again. If a process is found to be active it will be returned. The wait
// can also be interrupted by a CTRL + SHIFT + END key hit. If that happens, the function returns the end iterator.
std::forward_list<std::wstring>::const_iterator procWaitLoop(const std::forward_list<std::wstring> &procList)
{
	auto start(procList.cbegin());
	auto end(procList.cend());
	auto iter(Hook::checkProcessActive(start, end));

	while (iter == end)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_END) && GetAsyncKeyState(VK_SHIFT))
		{
			break;
		}

		iter = Hook::checkProcessActive(start, end);
	}

	return iter;
}

std::atomic<bool> earlyDie(false);

// Helper function for cleaning up the thread in use
inline void cleanupThread(std::thread &t)
{
	if (t.joinable())
	{
		t.join();
	}
	else if (t.get_id() != std::thread::id())
	{
		t.detach();
	}
}

// A function responsible for checking whether a process is still alive. Blocks completely and should be run on a
// separate thread. Set earlyDie to true to kill it early. This function will set earlyDie to true itself if the
// process in question did die and then exits.
void processCheckerThreadLoop(std::wstring processName)
{
	while (!earlyDie.load() && Hook::checkProcessAlive(processName))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	earlyDie.store(true);
}

void Hook::loop(Hook::Hid::HidControllerDevice &realD, Hook::Scp::VirtualDevice &vjd)
{
	auto mapper(Remapper::create(Remapper::DEFAULT_PATH));

	bool hasProcList(Config::get()->hasProcessList());
	const auto &procList(Config::get()->getProcessList());

	bool suicide(hasProcList ? Config::get()->shouldEndOnGameProcDeath() : false);
	std::thread worker;

	std::wstring activeProc(byteToWideString(Remapper::DEFAULT_PATH));

	if (!hasProcList)
	{
		std::cout << "\nSetup successful! Beginning controller translation. Press CTRL + END to quit. Press CTRL + HOME to re-read button mappings." << std::endl;
	}
	else
	{
		std::cout << "\nSetup successful! Awaiting game startup... Press CTRL + SHIFT + END to quit." << std::endl;
		auto procIter(procWaitLoop(procList));

		if (procIter == procList.cend())
		{
			return;
		}

		activeProc = *procIter;
		std::wcout << "Detected startup of " << activeProc << "." << std::endl;

		mapper = Remapper::create(activeProc);

		std::wcout << "Beginning controller translation. Press CTRL + END to quit. Press CTRL + HOME to re-read button mappings." << std::endl;

		worker = std::thread(processCheckerThreadLoop, activeProc);
	}

	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		realD.pullData();

		auto state(realD.parseData());

		mapper.remap(state);

		vjd.feed(state);

		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_HOME))
		{
			std::cout << "Re-reading button mappings from file...\n";
			Remapper::reread(activeProc, mapper);
		}
		if (GetAsyncKeyState(VK_END) && GetAsyncKeyState(VK_CONTROL))
		{
			earlyDie.store(true);

			cleanupThread(worker);

			if (hasProcList)
			{
				goto rewait;
			}
			break;
		}
		else if (hasProcList && earlyDie.load())
		{
			if (suicide)
			{
				cleanupThread(worker);
				std::cout << "Game process ended. Terminating controller translation and quitting." << std::endl;
				break;
			}

			std::cout << "Game process ended. Terminating controller translation." << std::endl;

		rewait:
			cleanupThread(worker);

			std::cout << "Awaiting game startup... Press CTRL + SHIFT + END to quit." << std::endl;

			auto procIter(procWaitLoop(procList));

			if (procIter == procList.cend())
			{
				break;
			}

			activeProc = *procIter;
			mapper = Remapper::create(activeProc);

			earlyDie.store(false);
			worker = std::thread(processCheckerThreadLoop, *procIter);
		}
	}
}

#pragma endregion