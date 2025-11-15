cmake_minimum_required(VERSION 3.30)
project(Dithering)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Dithering main.cpp
        Palette.cpp
        Palette.h
        Utilities.cpp
        Utilities.h
        Ditherer.cpp
        Ditherer.h)

target_link_libraries(Dithering ${OpenCV_LIBS})