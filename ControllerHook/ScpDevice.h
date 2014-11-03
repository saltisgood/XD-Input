#ifndef __CTRLHOOK_SCPDEVICE_H_
#define __CTRLHOOK_SCPDEVICE_H_

#include <guiddef.h>
#include <string>

#include "Controller.h"

namespace Hook
{
	namespace Scp
	{
		// Fix up the differences for the ScpDevice's ControllerState
		void fixDiscrepancies(ControllerState &state);

		class BusDevice;

		// A class that represents a single Virtual Joystick Device active in the Scp
		// driver.
		class VirtualDevice
		{
			friend class BusDevice;

			VirtualDevice(BusDevice& par, unsigned int devNum);

		public:
			// Feed the state of the controller to the Scp driver. Only works so long
			// as the parent BusDevice is active.
			void feed(ControllerState& state);

			// The number of the device (from 1 - 4).
			const unsigned int deviceNum;

		private:
			BusDevice& mParent;
		};

		// A class the handles the data transfer between VJDs and the Scp driver.
		class BusDevice
		{
			friend class VirtualDevice;

		public:
			BusDevice();
			~BusDevice();

			// Open a connection to the Scp driver. Call this before .start().
			bool open();
		private:
			// Open a connection to a device at a particular path
			bool open(const std::wstring& path);

		public:
			// Plugin the VJD to the Scp driver. If already active, does nothing.
			bool start();
			// Call to explicitly unplug the VJD and close the handle to the Scp driver. Called from the
			// destructor, so it's not necessary.
			bool stop();

			// Retrieve the VJD to feed data through. If the connection to the Scp driver is not active,
			// i.e. if you haven't called .open() && .start(), then this will throw a std::runtime_error.
			VirtualDevice& retrieveDevice();

		private:
			// Find a device with the given GUID and write the device's path into outPath. Use instance to
			// control the instance of the GUID to choose. Returns true if a device was found, false otherwise.
			bool find(const GUID *target, std::wstring& outPath, int instance = 0) const;

			// Open up a connection to the device at the given path.
			bool getDeviceHandle(const std::wstring& path);

			// Plugin a device to the Scp driver. Valid serial nos: 1-4.
			bool plugin(int serial) const;
			// Unplug a device from the Scp driver. Valid serial nos: 1-4.
			bool unplug(int serial) const;

			// Transfer a chunk of data to and from the Scp driver.
			bool transfer(void *input, size_t inputLen, void *output, size_t outputLen) const;

		private:
			GUID mClass;
			bool mIsActive;
			std::wstring mPath;
			void *mFileHandle;
			VirtualDevice mDevice;
		};

#pragma region inlines

		inline void fixDiscrepancies(ControllerState &state)
		{
			uint8_t swap(state.button_primary.button_y);
			state.button_primary.button_y = state.button_primary.button_x;
			state.button_primary.button_x = state.button_primary.button_a;
			state.button_primary.button_a = swap;
		}

#pragma endregion
	}
}

#endif