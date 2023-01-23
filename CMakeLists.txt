cmake_minimum_required(VERSION 3.21)

set(serial_VERSION 0.2.0)

project(serial
        VERSION ${serial_VERSION}
        DESCRIPTION "Windows serial communication"
        HOMEPAGE_URL "https://github.com/wilrickrt/serial"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
option(BUILD_EXAMPLES "Build examples" OFF)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

## Static builds on Windows
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include_directories(include)
add_subdirectory(src)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


## Project installation
configure_package_config_file(cmake/config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/wrt-serial-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/wrt-serial
	NO_SET_AND_CHECK_MACRO)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/wrt-serial-config-version.cmake
	VERSION ${serial_VERSION}
	COMPATIBILITY SameMajorVersion)

# CMake files
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/wrt-serial-config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/wrt-serial-config-version.cmake
	DESTINATION
		${CMAKE_INSTALL_DATADIR}/wrt-serial)
# Include directories
install(
    DIRECTORY include/ 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# Targets
install(EXPORT wrt-serial-targets
	NAMESPACE wrt::
	DESTINATION ${CMAKE_INSTALL_DATADIR}/wrt-serial)
