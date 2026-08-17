cmake_minimum_required(VERSION 3.18)
project(VideoEncoder CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. CUDA Toolkit suchen
find_package(CUDAToolkit REQUIRED)

# 2. OpenCV mit CUDA-Komponenten suchen
find_package(OpenCV REQUIRED COMPONENTS core imgproc videoio cudaarithm cudaimgproc)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
)

add_executable(video_encoder
    main.cpp
    encoder.cpp
)

# 3. Bibliotheken verknüpfen
target_link_libraries(video_encoder PRIVATE 
    ${OpenCV_LIBS}
    CUDA::cudart
)