#include "HID_Util.h"

#include "Controller.h"
#include "ScpDevice.h"

#include <Windows.h>

extern "C"
{
	#include <Hidsdi.h>
}

using namespace Hook::Hid;

HidControllerDevice::HidControllerDevice(HidDevice details) :
	mDetails(details),
	mReadHandle(INVALID_HANDLE_VALUE),
	mIsOpen(false),
	mInputData(nullptr)
{
	auto handle(openHandle(mDetails.path.data(), false));

	mAttrs = getDeviceAttributes(handle);
	mCapabilities = getDeviceCapabilities(handle);

	CloseHandle(handle);

	mInputData = new unsigned char[mCapabilities.InputReportByteLength];
}

HidControllerDevice::~HidControllerDevice()
{
	closeDevice();

	delete[] mInputData;
}

HidControllerDevice::WIN_HANDLE HidControllerDevice::openHandle(const wchar_t *path, bool isExcl)
{
	if (isExcl)
	{
		return CreateFile(path, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	}
	else
	{
		return CreateFile(path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	}
}

HidControllerDevice::HIDD_ATTRIBUTES HidControllerDevice::getDeviceAttributes(HidControllerDevice::WIN_HANDLE handle)
{
	HIDD_ATTRIBUTES attrs;
	ZeroMemory(&attrs, sizeof(HIDD_ATTRIBUTES));

	attrs.Size = sizeof(HIDD_ATTRIBUTES);

	HidD_GetAttributes(handle, reinterpret_cast<::HIDD_ATTRIBUTES *>(&attrs));

	return attrs;
}

HidControllerDevice::HIDP_CAPS HidControllerDevice::getDeviceCapabilities(HidControllerDevice::WIN_HANDLE handle)
{
	HIDP_CAPS caps;
	ZeroMemory(&caps, sizeof(HIDP_CAPS));

	PHIDP_PREPARSED_DATA preparsedDataP(NULL);

	if (HidD_GetPreparsedData(handle, &preparsedDataP))
	{
		HidP_GetCaps(preparsedDataP, reinterpret_cast<PHIDP_CAPS>(&caps));

		HidD_FreePreparsedData(preparsedDataP);
	}

	return caps;
}

bool HidControllerDevice::openDevice(bool isExcl)
{
	if (mIsOpen)
	{
		return true;
	}

	return mIsOpen = ((mReadHandle = openHandle(mDetails.path.data(), isExcl)) != INVALID_HANDLE_VALUE);
}

void HidControllerDevice::closeDevice()
{
	if (mIsOpen)
	{
		CloseHandle(mReadHandle);

		mReadHandle = INVALID_HANDLE_VALUE;
		mIsOpen = false;
	}
}

void HidControllerDevice::pullData()
{
	if (!mIsOpen)
	{
		return;
	}

	DWORD bytesRead(0);
	BOOL result = ReadFile(mReadHandle, mInputData, mCapabilities.InputReportByteLength, &bytesRead, NULL);
}

// NOTE!
// This function is designed to work with 1 particular device! It is NOT guaranteed to
// work with any others! Proceed with caution and check different devices for different
// data
Hook::ControllerState HidControllerDevice::parseData()
{
	ControllerState state;
	ZeroMemory(&state, sizeof(ControllerState));
	initialiseState(state, 1);

#ifdef _DEBUG
	// For debug purposes it's nice to be able to see the whole of the array
	#define BUFF buff
	BYTE BUFF[9];

	for (int i = 0; i < 9; ++i)
	{
		BUFF[i] = mInputData[i];
	}
#else
	#define BUFF mInputData
#endif

#define HOOK_BIT_CHECK(byteNo, bitNo) (BUFF[byteNo] & (0x1 << bitNo))
#define HOOK_BIT_SET(byteNo, bitNo) ((BUFF[byteNo] & (0x1 << bitNo)) >> bitNo)

	state.button_primary.button_y = HOOK_BIT_SET(6, 4);
	state.button_primary.button_b = HOOK_BIT_SET(6, 5);
	state.button_primary.button_a = HOOK_BIT_SET(6, 6);
	state.button_primary.button_x = HOOK_BIT_SET(6, 7);

	state.button_primary.left_shoulder = HOOK_BIT_SET(7, 0);
	state.button_primary.right_shoulder = HOOK_BIT_SET(7, 1);
	
	state.left_trigger = HOOK_BIT_CHECK(7, 2) ? UCHAR_MAX : 0;
	state.right_trigger = HOOK_BIT_CHECK(7, 3) ? UCHAR_MAX : 0;

	state.button_secondary.back = HOOK_BIT_SET(7, 4);
	state.button_secondary.start = HOOK_BIT_SET(7, 5);

	state.button_secondary.left_stick = HOOK_BIT_SET(7, 6);
	state.button_secondary.right_stick = HOOK_BIT_SET(7, 7);

	BYTE hatByte(BUFF[6] & 0xF);

	if (hatByte != 0xF)
	{
		if (hatByte < 0x2 || hatByte > 0x6)
		{
			state.button_secondary.hat_up = 1;
		}

		if (hatByte > 0x0 && hatByte < 0x4)
		{
			state.button_secondary.hat_right = 1;
		}

		if (hatByte > 0x2 && hatByte < 0x6)
		{
			state.button_secondary.hat_down = 1;
		}

		if (hatByte > 0x4)
		{
			state.button_secondary.hat_left = 1;
		}
	}

	state.left_stick_x = (BUFF[1] << 8) - SHRT_MAX;
	state.left_stick_y = - static_cast<int16_t>((BUFF[2] << 8) - SHRT_MAX);

	state.right_stick_x = (BUFF[4] << 8) - SHRT_MAX;
	state.right_stick_y = - static_cast<int16_t>((BUFF[5] << 8) - SHRT_MAX);

	Scp::fixDiscrepancies(state);

	return state;
}