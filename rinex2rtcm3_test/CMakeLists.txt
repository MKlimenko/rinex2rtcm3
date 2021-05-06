cmake_minimum_required (VERSION 3.8)

# Setup GoogleTest
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
	message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
	message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)


project(rinex2rtcm3_test)
add_executable (${PROJECT_NAME} 
							rinex2rtcm3_test.cpp 
							../rinex2rtcm3/rinex2rtcm3.hpp
							../RTKLIB/src/geoid.c
							../RTKLIB/src/rcvraw.c
							../RTKLIB/src/rinex.c
							../RTKLIB/src/rtcm.c
							../RTKLIB/src/rtcm2.c
							../RTKLIB/src/rtcm3.c
							../RTKLIB/src/rtcm3e.c
							../RTKLIB/src/rtkcmn.c
							../RTKLIB/src/sbas.c
							../RTKLIB/src/solution.c
							../RTKLIB/src/stream.c
							../RTKLIB/src/streamsvr.c
							../RTKLIB/src/rcv/binex.c
							../RTKLIB/src/rcv/crescent.c							
							../RTKLIB/src/rcv/javad.c
							../RTKLIB/src/rcv/novatel.c
							../RTKLIB/src/rcv/nvs.c
							../RTKLIB/src/rcv/rt17.c
							../RTKLIB/src/rcv/septentrio.c
							../RTKLIB/src/rcv/skytraq.c
							../RTKLIB/src/rcv/ss2.c
							../RTKLIB/src/rcv/ublox.c	
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PUBLIC ../RTKLIB/src)
target_include_directories(${PROJECT_NAME} PUBLIC ../Lyra/include)
target_include_directories(${PROJECT_NAME} PUBLIC ../rinex2rtcm3)

if (WIN32)
	target_link_libraries(${PROJECT_NAME} winmm ws2_32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS WIN32)
else()
	target_link_libraries(${PROJECT_NAME} pthread)
endif (WIN32)

target_link_libraries(${PROJECT_NAME} gtest_main)
target_compile_definitions(${PROJECT_NAME} PUBLIC TRACE QT ENAGLO ENAGAL ENAQZS ENACMP ENAIRN ENALEO)

enable_testing()
add_test(NAME rinex2rtcm3_test COMMAND rinex2rtcm3_test)