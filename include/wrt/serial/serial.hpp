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
		/**
		 * @brief Constructor
		 *
		 * @param handle The handle to the serial file
		 * @param port The communication port number
		 * @param baudRate The configured baud rate
		 */
		Serial(HANDLE handle, int port, int baudRate = CBR_115200);
		Serial(Serial&& other) noexcept;
		Serial(Serial&) = delete;
		Serial& operator=(Serial&& other) noexcept;
		Serial& operator=(Serial&) = delete;
		virtual ~Serial();

		/**
		 * @brief Configure the baud rate
		 *
		 * @param baudRate The new baud rate
		 * @return `true` if succeeded, else `false`
		 */
		bool SetBaudRate(int baudRate);

		/**
		 * @brief Configure the serial timeouts
		 */
		bool SetTimeouts(
			DWORD readIntervalTimeout = 50,
			DWORD readTotalTimeoutConstant = 50,
			DWORD readTotalTimeoutMultiplier = 10,
			DWORD writeTotalTimeoutConstant = 50,
			DWORD writeTotalTimeoutMultiplier = 10);

		/**
		 * @brief Write integer to serial
		 *
		 * @param value The value to write
		 * @return `true` if succeeded, else `false`
		 */
		bool Write(int value);

		/**
		 * @brief Write string to serial
		 *
		 * @param text The text to write
		 * @return `true` if succeeded, else `false`
		 */
		bool Write(const std::string& text);

	private:
		HANDLE m_handle;	// Handle to serial communication file
		int m_port;			// The communication port number
		int m_baudRate;		// The configured baud rate 
	};

	/**
	 * @brief Open serial communication
	 *
	 * @param vendorId The vendor id as a hex string
	 * @param productId The product id as a hex string
	 * @return Serial communication
	 */
	std::optional<Serial> OpenSerial(
		const std::string& vendorId,
		const std::string& productId);

	/**
	 * @brief Open serial communication
	 *
	 * The hardware format should be: USB\VID_1234&PID_5678
	 *
	 * @param hardwareId The hardware id as uppercase string
	 * @return Serial communication
	 */
	std::optional<Serial> OpenSerial(
		const std::string& hardwareId);

	/**
	 * @brief Open serial communication
	 *
	 * @param comPort The communication port number
	 * @return Serial communication
	 */
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
	 * The hardware format should be: USB\VID_1234&PID_5678
	 *
	 * @param hardwareId The hardware id as uppercase string
	 * @return The port number
	 */
	std::optional<int> FindComPort(
		const std::string& hardwareId);
}

#endif // WRT_SERIAL_SERIAL_HPP
