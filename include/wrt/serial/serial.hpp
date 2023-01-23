#ifndef WRT_SERIAL_SERIAL_HPP
#define WRT_SERIAL_SERIAL_HPP

#include <Windows.h>
#include <Setupapi.h>

#include <optional>
#include <string>

namespace wrt
{
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
