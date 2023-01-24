cmake_minimum_required(VERSION 3.21)
project(measurements)

set(CMAKE_CXX_STANDARD 23)

add_executable(measurements
    room-correction.cpp
    SweepGenerator.cpp
    ALSAPCMDevice.cpp
    ALSACaptureDevice.cpp
    ALSAPlaybackDevice.cpp
    ImpulseResponseConverter.cpp
    drc/fftsg_h.c
        )

target_link_libraries(measurements asound pthread)