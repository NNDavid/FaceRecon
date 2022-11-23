project(FaceReconDesktop)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenCV REQUIRED)

add_executable(FaceReconDesktop MainWindow.h MainWindow.cpp main.cpp)
target_link_libraries(FaceReconDesktop PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets ${OpenCV_LIBS})
