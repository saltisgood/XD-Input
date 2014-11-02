#ifndef __CTRLHOOK_LOOPER_H_
#define __CTRLHOOK_LOOPER_H_

#include "HID_Util.h"
#include "ScpDevice.h"

#include <map>

namespace Hook
{
	class Remapper
	{
	public:
		const static std::string DEFAULT_PATH;

		static Remapper create(const std::string& filePath);
		static Remapper create(const std::wstring &filePath);

		static bool reread(const std::wstring &filePath, Remapper &mapper);

		static void printTemplate(const std::string& filePath);

	private:
		Remapper();

	public:
		void remap(ControllerState &state) const;

	private:
		void remapPriv(ControllerState &state) const;

	private:
		bool mIsDefault;
		
		std::map<Buttons, Buttons> mMappings;
	};

	void loop(Hid::HidControllerDevice &realDevice, Scp::VirtualDevice &virtualDevice);

#pragma region inlines

	inline void Remapper::remap(ControllerState &state) const
	{
		if (!mIsDefault)
		{
			remapPriv(state);
		}
	}

#pragma endregion
}

#endif