cmake_minimum_required(VERSION 3.22)
project(FaceRecon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

add_subdirectory(3rdparty)
add_subdirectory(common_libraries)
add_subdirectory(FaceReconDesktop)
add_subdirectory(FaceReconServer)
