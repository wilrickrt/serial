#ifndef WRT_SERIAL_SERIAL_HPP
#define WRT_SERIAL_SERIAL_HPP

#include <Windows.h>
#include <Setupapi.h>

#include <optional>
#include <string>

namespace wrt
{
	class Serial
	{
	public:
		Serial(HANDLE handle, int port, int baudRate = CBR_115200);
		Serial(Serial&& other) noexcept;
		Serial(Serial& other) = delete;
		Serial& operator=(Serial&& other) noexcept;
		Serial& operator=(Serial&) = delete;
		virtual ~Serial();

		bool SetBaudRate(int baudRate);
		bool SetTimeouts();

		bool Write(int value);
		bool Write(const std::string& text);

	private:
		HANDLE m_handle;
		int m_port;
		int m_baudRate;
	};

	std::optional<Serial> OpenSerial(
		const std::string& vendorId,
		const std::string& productId);

	std::optional<Serial> OpenSerial(int comPort);

	/**
	 * @brief Create hardware id from vendor id and product id
	 *
	 * @param vendorId The vendor id as a hex string
	 * @param productId The product id as a hex string
	 * @return The hardware id
	 */
	std::string CreateHardwareID(
		const std::string& vendorId,
		const std::string& productId);

	/**
	 * @brief Find a COM port
	 *
	 * @param vendorId The vendor id as a hex string
	 * @param productId The product id as a hex string
	 * @return The port number
	 */
	std::optional<int> FindComPort(
		const std::string& vendorId, 
		const std::string& productId);

	/**
	 * @brief Find a COM port
	 *
	 * The hardware format should be: USB\VID_1234&PID_123
	 *
	 * @param hardwareId The hardware id as uppercase string
	 * @return The port number
	 */
	std::optional<int> FindComPort(
		const std::string& hardwareId);
}

#endif // WRT_SERIAL_SERIAL_HPP
