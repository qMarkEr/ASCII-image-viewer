cmake_minimum_required(VERSION 3.28)
project(igotpicturesinmyterminal)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        src/stb_image.h
        src/main.cpp
        src/Image.cpp
        src/Image.h
)
