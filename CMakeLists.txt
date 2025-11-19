cmake_minimum_required(VERSION 3.30)
project(Dithering)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ImageProcessing imageProcessing.cpp
        Palette.cpp
        Palette.h
        Ditherer.cpp
        Ditherer.h)

add_executable(VideoProcessing videoProcessing.cpp
        Palette.cpp
        Palette.h
        Ditherer.cpp
        Ditherer.h)

target_link_libraries(ImageProcessing ${OpenCV_LIBS})
target_link_libraries(VideoProcessing ${OpenCV_LIBS})