cmake_minimum_required(VERSION 3.22)
project(FaceReconDesktop)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} MainWindow.h MainWindow.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets ${OpenCV_LIBS} common_libraries)