# compile main.cpp\avcodec_encode.cpp to executable main

cmake_minimum_required(VERSION 3.0)

project(Project1)

# add the executable

add_executable(main main.cpp avcodec_encode.cpp)

# link the libraries

target_link_libraries(main avcodec avutil)

# set the include directories

target_include_directories(main PRIVATE /usr/include/x86_64-linux-gnu)


