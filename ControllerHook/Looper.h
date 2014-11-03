#ifndef __CTRLHOOK_LOOPER_H_
#define __CTRLHOOK_LOOPER_H_

#include "HID_Util.h"
#include "ScpDevice.h"

#include <map>

namespace Hook
{
	// The class used for remapping buttons on the controller.
	class Remapper
	{
	public:
		// The relative path to the default config file.
		const static std::string DEFAULT_PATH;

		// Create a Remapper object based on a config file at the specified path.
		static Remapper create(const std::string &filePath);
		// Create a Remapper object based on a config file at the specified path.
		static Remapper create(const std::wstring &filePath);

		// Reset the given Remapper object with updated info from the config file at the
		// specified path. If the file can't be found or read from, the object is left
		// unchanged and the function returns false, true otherwise.
		static bool reread(const std::wstring &filePath, Remapper &mapper);

		// Print the default config file form at the specified path.
		static void printTemplate(const std::string& filePath);

	private:
		Remapper();

	public:
		// Remap the buttons in the given state.
		void remap(ControllerState &state) const;

	private:
		void remapPriv(ControllerState &state) const;

	private:
		bool mIsDefault;
		
		std::map<Buttons, Buttons> mMappings;
	};

	// Start performing the controller translation operation.
	// NOT THREAD SAFE! DO NOT CALL MULTIPLE INSTANCES SIMULTANEOUSLY!
	void loop(Hid::HidControllerDevice &realDevice, Scp::VirtualDevice &virtualDevice);

#pragma region inlines

	inline void Remapper::remap(ControllerState &state) const
	{
		// Only bother remapping if there's a non-default state
		if (!mIsDefault)
		{
			remapPriv(state);
		}
	}

#pragma endregion
}

#endif