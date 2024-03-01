cmake_minimum_required(VERSION 3.27)
project(FAAD_TEST)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE faad_srcs CONFIGURE_DEPENDS src/*.cpp src/*.c src/*.h include/*.h)
add_library(faad SHARED ${faad_srcs})
target_include_directories(faad PUBLIC include)

#add_executable(FAAD_TEST main.cpp)
