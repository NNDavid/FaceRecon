project(FaceReconServer)
find_package(Crow)
add_executable(FaceReconServer main.cpp)
target_link_libraries(FaceReconServer PRIVATE Crow)
