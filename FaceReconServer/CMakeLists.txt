cmake_minimum_required(VERSION 3.22)
project(FaceReconServer)

find_package(CURL)
find_package(cpprestsdk REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED) 

add_executable(${PROJECT_NAME} Data.h ServerBackend.h ServerBackend.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Crow CURL::libcurl cpprestsdk::cpprest ${OpenCV_LIBS} ${Boost_LIBRARIES} common_libraries)