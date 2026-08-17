cmake_minimum_required(VERSION 3.12)
project(VideoEncoder CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(video_encoder
    main.cpp
    encoder.cpp
)

target_link_libraries(video_encoder PRIVATE ${OpenCV_LIBS})