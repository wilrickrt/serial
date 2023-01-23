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

	Serial::Serial(HANDLE handle, int port, int baudRate)
		: m_handle(handle), m_port(port), m_baudRate(baudRate)
	{}

	Serial::Serial(Serial&& other) noexcept
		: Serial(other.m_handle, other.m_port, other.m_baudRate)
	{
		other.m_handle = 0;
	}

	Serial& Serial::operator=(Serial&& other) noexcept
	{
		if (this != &other)
		{
			m_handle = other.m_handle;
			m_port = other.m_port;
			m_baudRate = other.m_baudRate;
			other.m_handle = 0;
		}
		return *this;
	}

	Serial::~Serial()
	{
		if (m_handle)
		{
			CloseHandle(m_handle);
		}
	}

	bool Serial::SetBaudRate(int baudRate)
	{
		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		if (!GetCommState(m_handle, &dcbSerialParams))
		{
			return false; // unable to get current commstate
		}

		dcbSerialParams.BaudRate = baudRate;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		if (!SetCommState(m_handle, &dcbSerialParams))
		{
			return false; // unable to set new commstate
		}

		m_baudRate = baudRate;

		return true;
	}

	bool Serial::SetTimeouts(
		DWORD readIntervalTimeout,
		DWORD readTotalTimeoutConstant,
		DWORD readTotalTimeoutMultiplier,
		DWORD writeTotalTimeoutConstant,
		DWORD writeTotalTimeoutMultiplier
	)
	{
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = readIntervalTimeout;
		timeouts.ReadTotalTimeoutConstant = readTotalTimeoutConstant;
		timeouts.ReadTotalTimeoutMultiplier = readTotalTimeoutMultiplier;
		timeouts.WriteTotalTimeoutConstant = writeTotalTimeoutConstant;
		timeouts.WriteTotalTimeoutMultiplier = writeTotalTimeoutMultiplier;

		return SetCommTimeouts(m_handle, &timeouts);
	}

	bool Serial::Write(int value)
	{
		DWORD written;
		WriteFile(m_handle,
			(void*)value,
			sizeof(value),
			&written,
			nullptr);
		return written == sizeof(value);
	}

	bool Serial::Write(const std::string& text)
	{
		DWORD written;
		WriteFile(m_handle,
			text.c_str(),
			text.size(),
			&written,
			nullptr);
		return written == text.size();
	}

	std::optional<Serial> OpenSerial(
		const std::string& vendorId, 
		const std::string& productId)
	{
		return OpenSerial(CreateHardwareID(vendorId, productId));
	}

	std::optional<Serial> OpenSerial(const std::string& hardwareId)
	{
		if (auto port = FindComPort(hardwareId); port)
		{
			return OpenSerial(*port);
		}
		return {};
	}

	std::optional<Serial> OpenSerial(int comPort)
	{
		std::string portName{ "\\\\.\\COM" + std::to_string(comPort) };
		HANDLE handle = CreateFile(portName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr);
		if (handle == INVALID_HANDLE_VALUE)
		{
			return {}; // failed to open the port
		}

		Serial serial(handle, comPort);
		if (serial.SetBaudRate(CBR_115200) && serial.SetTimeouts())
		{
			return serial;
		}

		return {}; // failed to initialize
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
