project(FaceReconDesktop)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenCV REQUIRED)
find_package(CUDNN REQUIRED)

add_executable(FaceReconDesktop NeuralNetwork.h NeuralNetwork.cpp MainWindow.h MainWindow.cpp main.cpp)
target_link_libraries(FaceReconDesktop PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets ${OpenCV_LIBS} Crow CuDNN::CuDNN)
