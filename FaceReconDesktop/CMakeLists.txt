cmake_minimum_required(VERSION 3.22)
project(FaceReconDesktop)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenCV REQUIRED)
find_package(CURL)

add_executable(${PROJECT_NAME} MainWindow.h MainWindow.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets ${OpenCV_LIBS} common_libraries CURL::libcurl)