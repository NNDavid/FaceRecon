cmake_minimum_required(VERSION 3.22)
project(common_libraries)

find_package(OpenCV REQUIRED)

set(SOURCES src/FaceImageEvaluator.cpp src/ThreadSafeFaceImageEvaluator.cpp src/utilities.cpp)

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})