add_library(serial serial.cpp)
target_link_libraries(serial Setupapi)
target_include_directories(serial
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
add_library(wrt::serial ALIAS serial)

install(TARGETS serial EXPORT wrt-serial-targets)
