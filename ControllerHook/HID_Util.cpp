#include "HID_Util.h"

#include <Windows.h>
#include <SetupAPI.h>
#include <winusb.h>

#include <hidclass.h>

using namespace Hook::Hid;

const GUID HID_GUID = GUID_DEVINTERFACE_HID;

std::forward_list<HidDevice> Hook::Hid::enumerateHIDDevices(int vendId, int prodIds, ...)
{
	return enumerateDevices(HID_GUID, vendId, prodIds);
}

std::forward_list<HidDevice> Hook::Hid::enumerateHIDDevicesAlt()
{
	return enumerateDevicesAlt(HID_GUID);
}

std::forward_list<HidDevice> Hook::Hid::enumerateDevices(const GUID &target, int vendId, int prodIds, ...)
{
	std::forward_list<HidDevice> rtList;

	HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&target, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	INT32 memberIndex = 0;
	DWORD bufferSize = 0;

	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	SP_DEVINFO_DATA da;
	da.cbSize = sizeof(SP_DEVINFO_DATA);

	while (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &target, memberIndex, &deviceInterfaceData))
	{
		SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &bufferSize, &da);

		PSP_DEVICE_INTERFACE_DETAIL_DATA detailDataBuffer = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(new char[bufferSize]);
		detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, bufferSize, &bufferSize, &da))
		{
			HidDevice dev;
			dev.path.assign(detailDataBuffer->DevicePath);

			rtList.insert_after(rtList.before_begin(), std::move(dev));

			delete detailDataBuffer;
		}

		++memberIndex;
	}

	if (deviceInfoSet)
	{
		SetupDiDestroyDeviceInfoList(deviceInfoSet);
	}

	return rtList;
}

std::wstring getDevicePath(HDEVINFO deviceInfoSet, SP_DEVICE_INTERFACE_DATA &deviceInterfaceData)
{
	DWORD bufferSize = 0;

	SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &bufferSize, NULL);

	PSP_DEVICE_INTERFACE_DETAIL_DATA detailDataBuffer = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(new char[bufferSize]);
	detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, bufferSize, &bufferSize, NULL);

	std::wstring rt(detailDataBuffer->DevicePath);

	delete detailDataBuffer;

	return rt;
}

std::wstring getBusReportedDeviceDescription(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA &devInfoData)
{
	BYTE descriptionBuffer[1024];

	// Ignore OS check

	DEVPROPKEY busReportedDevDescKey;
	busReportedDevDescKey.pid = 4;
	busReportedDevDescKey.fmtid.Data1 = 0x540b947e;
	busReportedDevDescKey.fmtid.Data2 = 0x8b40;
	busReportedDevDescKey.fmtid.Data3 = 0x45bc;
	busReportedDevDescKey.fmtid.Data4[0] = 0xa8;
	busReportedDevDescKey.fmtid.Data4[1] = 0xa2;
	busReportedDevDescKey.fmtid.Data4[2] = 0x6a;
	busReportedDevDescKey.fmtid.Data4[3] = 0x0b;
	busReportedDevDescKey.fmtid.Data4[4] = 0x89;
	busReportedDevDescKey.fmtid.Data4[5] = 0x4c;
	busReportedDevDescKey.fmtid.Data4[6] = 0xbd;
	busReportedDevDescKey.fmtid.Data4[7] = 0xa2;

	DEVPROPTYPE propertyType;
	DWORD requiredSize;

	BOOL result = SetupDiGetDeviceProperty(deviceInfoSet, &devInfoData, &busReportedDevDescKey, &propertyType,
		descriptionBuffer, 1024, &requiredSize, 0);

	if (result != FALSE)
	{
		return std::wstring(reinterpret_cast<wchar_t *>(descriptionBuffer));
	}

	return std::wstring();
}

std::wstring getDeviceDescription(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA &deviceInfoData)
{
	BYTE descriptionBuffer[1024];

	DWORD type(0);
	DWORD requiredSize(0);

	BOOL result = SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, &type, descriptionBuffer, 1024, &requiredSize);

	if (result != FALSE)
	{
		return std::wstring(reinterpret_cast<wchar_t *>(descriptionBuffer));
	}

	return std::wstring();
}

std::forward_list<HidDevice> Hook::Hid::enumerateDevicesAlt(const GUID &guid)
{
	std::forward_list<HidDevice> rtList;

	HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (deviceInfoSet != INVALID_HANDLE_VALUE)
	{
		SP_DEVINFO_DATA deviceInfoData;
		deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		deviceInfoData.DevInst = 0;
		deviceInfoData.ClassGuid.Data1 = 0;
		deviceInfoData.ClassGuid.Data2 = 0;
		deviceInfoData.ClassGuid.Data3 = 0;
		for (int i = 0; i < 8; ++i)
		{
			deviceInfoData.ClassGuid.Data4[i] = 0;
		}

		deviceInfoData.Reserved = NULL;

		DWORD deviceIndex = 0;

		while (SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData))
		{
			++deviceIndex;

			SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
			deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

			DWORD deviceInterfaceIndex = 0;

			while (SetupDiEnumDeviceInterfaces(deviceInfoSet, &deviceInfoData, &guid, deviceInterfaceIndex,
				&deviceInterfaceData))
			{
				++deviceInterfaceIndex;

				std::wstring devicePath = getDevicePath(deviceInfoSet, deviceInterfaceData);
				std::wstring description = getBusReportedDeviceDescription(deviceInfoSet, deviceInfoData);

				if (description.empty())
				{
					description = getDeviceDescription(deviceInfoSet, deviceInfoData);
				}

				HidDevice device;
				device.description = description;
				device.path = devicePath;

				rtList.push_front(std::move(device));
			}
		}
	}

	return rtList;
}