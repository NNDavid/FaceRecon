cmake_minimum_required(VERSION 3.23)
project(FaceRecon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)


add_executable(FaceRecon MainWindow.h MainWindow.cpp main.cpp)
target_link_libraries(FaceRecon PRIVATE Qt6::Core Qt6::Widgets)
