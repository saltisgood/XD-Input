#include "Looper.h"

#include "Util.h"

#include <Windows.h>
#include <fstream>
#include <iostream>

#pragma region Helper

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

Hook::Remapper Hook::Remapper::create(const std::string& filePath)
{
	Remapper rtn;

	std::ifstream file(filePath);

	std::cout << "Remapping buttons... ";

	if (!file)
	{
		std::cout << filePath << " config file not found! Using default values." << std::endl;

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

void Hook::loop(Hook::Hid::HidControllerDevice &realD, Hook::Scp::VirtualDevice &vjd)
{
	auto mapper(Remapper::create(Remapper::DEFAULT_PATH));

	std::cout << "\nSetup successful! Beginning controller translation. Press CTRL + END to end." << std::endl;

	for (;;)
	{
		Sleep(1);

		realD.pullData();

		auto state(realD.parseData());

		mapper.remap(state);

		vjd.feed(state);

		if (GetAsyncKeyState(VK_END) && GetAsyncKeyState(VK_CONTROL))
		{
			break;
		}
	}
}