cmake_minimum_required(VERSION 3.22)
project(FaceReconServer)

find_package(CURL)
find_package(cpprestsdk REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Crow CURL::libcurl cpprestsdk::cpprest ${OpenCV_LIBS} common_libraries)