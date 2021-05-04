cmake_minimum_required (VERSION 3.8)

add_executable (rinex2rtcm3 "rinex2rtcm3.cpp")

target_include_directories(rinex2rtcm3 PUBLIC ../RTKLIB/src)
target_include_directories(rinex2rtcm3 PUBLIC ../Lyra/include)
