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

		class VirtualDevice
		{
			friend class BusDevice;

			VirtualDevice(BusDevice& par, unsigned int devNum);

		public:
			void feed(ControllerState& state);

			const unsigned int deviceNum;

		private:
			BusDevice& mParent;
		};

		class BusDevice
		{
			friend class VirtualDevice;

		public:
			BusDevice();
			~BusDevice();

			bool open(int instance = 0);
			bool open(const std::wstring& path);

			bool start();
			bool stop();

			VirtualDevice& retrieveDevice();

		private:
			bool find(const GUID *target, std::wstring& outPath, int instance = 0);
			bool getDeviceHandle(const std::wstring& path);

			bool plugin(int serial);
			bool unplug(int serial);

			bool transfer(void *input, size_t inputLen, void *output, size_t outputLen);

		private:
			GUID mClass;
			void *mWinUsbHandle;
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