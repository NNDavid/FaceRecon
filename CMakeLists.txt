project(FaceRecon)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

add_subdirectory(3rdparty)
add_subdirectory(FaceReconDesktop)
add_subdirectory(FaceReconServer)
