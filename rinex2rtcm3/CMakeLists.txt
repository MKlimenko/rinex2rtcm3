cmake_minimum_required (VERSION 3.8)

add_executable (rinex2rtcm3 
							rinex2rtcm3.cpp 
							rinex2rtcm3.hpp
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

set_property(TARGET rinex2rtcm3 PROPERTY CXX_STANDARD 20)

target_include_directories(rinex2rtcm3 PUBLIC ../RTKLIB/src)
target_include_directories(rinex2rtcm3 PUBLIC ../Lyra/include)

if (WIN32)
	target_link_libraries(${PROJECT_NAME} winmm ws2_32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS WIN32)
endif (WIN32)

target_compile_definitions(${PROJECT_NAME} PUBLIC TRACE QT ENAGLO ENAGAL ENAQZS ENACMP ENAIRN ENALEO)
