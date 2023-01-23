#include "wrt/serial/serial.hpp"

#include <algorithm>

namespace wrt
{
	std::optional<int> to_int(const std::string& string)
	{
		int result = 0;
		for (char c : string)
		{
			if (c >= '0' && c <= '9')
			{
				result = result * 10 + c - '0';
			}
			else
			{
				return {};
			}
		}

		return result;
	}

	std::string CreateHardwareID(
		const std::string& vendorId, 
		const std::string& productId)
	{
		std::string hardwareId{ "usb\\vid_" + vendorId + "&pid_" + productId };
		std::ranges::transform(
			hardwareId.begin(), 
			hardwareId.end(), 
			hardwareId.begin(), 
			std::toupper);
		return hardwareId;
	}

	std::optional<int> FindComPort(
		const std::string& vendorId,
		const std::string& productId)
	{
		return FindComPort(CreateHardwareID(vendorId, productId));
	}

	std::optional<int> FindComPort(
		const std::string& hardwareId)
	{
		std::string portName;

		HDEVINFO deviceInfoSet;
		DWORD deviceIndex = 0;
		SP_DEVINFO_DATA deviceInfoData;
		std::string deviceEnum = "USB";
		BYTE szBuffer[1024] = { 0 };
		DEVPROPTYPE ulPropertyType;
		DWORD dwSize = 0;

		deviceInfoSet = SetupDiGetClassDevs(
			NULL,
			deviceEnum.c_str(),
			NULL,
			DIGCF_ALLCLASSES | DIGCF_PRESENT);
		if (deviceInfoSet == INVALID_HANDLE_VALUE)
		{
			return {}; // failed to get device info set
		}

		ZeroMemory(&deviceInfoData, sizeof(SP_DEVINFO_DATA));
		deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		while (SetupDiEnumDeviceInfo(
			deviceInfoSet,
			deviceIndex,
			&deviceInfoData))
		{
			if (SetupDiGetDeviceRegistryProperty(
				deviceInfoSet,
				&deviceInfoData,
				SPDRP_HARDWAREID,
				&ulPropertyType,
				szBuffer,
				sizeof(szBuffer),
				&dwSize))
			{
				if (std::string{(char*)szBuffer}.starts_with(hardwareId))
				{
					HKEY hDeviceRegistryKey = SetupDiOpenDevRegKey(
						deviceInfoSet, 
						&deviceInfoData, 
						DICS_FLAG_GLOBAL, 
						0, 
						DIREG_DEV, 
						KEY_READ);
					if (hDeviceRegistryKey == INVALID_HANDLE_VALUE)
					{
						SetupDiDestroyDeviceInfoList(deviceInfoSet);
						return {}; // failed to open registry
					}

					char pszPortName[20];
					DWORD dwSize = sizeof(pszPortName);
					DWORD dwType = 0;
					if ((RegQueryValueEx(hDeviceRegistryKey, "PortName", nullptr, &dwType, (LPBYTE)pszPortName, &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
					{
						portName = std::string{ pszPortName };
						RegCloseKey(hDeviceRegistryKey);
						break;
					}
						
					RegCloseKey(hDeviceRegistryKey);
					return {}; // device has no portname
				}
			}

			deviceIndex++;
		}

		SetupDiDestroyDeviceInfoList(deviceInfoSet);

		if (portName.starts_with("COM"))
		{
			if (auto number = to_int(portName.substr(3)))
			{
				return { *number };
			}
		}

		return {}; // invalid portName string
	}
}
