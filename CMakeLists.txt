cmake_minimum_required(VERSION 3.20)
project(drpc)

set(CMAKE_BUILD_TYPE "Debug")
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

#设置项目编译头文件搜索路径 -I
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

#设置项目库文件搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(${CMAKE_SOURCE_DIR}/example)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)