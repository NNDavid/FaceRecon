# FaceRecon
## FaceReconServer
A webservice dedicated to handle registration with face-detection and face-recognition.
## FaceReconDesktop
A desktop application dedicated to facilitate entry to an event using face-detection and face-recognition
## BUILD
### REQUIREMENTS
* MSVC compiler
* Qt6
* Vcpkg

### INSTRUCTIONS
Clone the subrepositories <code>git submodule update --init</code> 
The <i>scripts/build.sh</i> shell script should be used for building this project. 
* Set the <b>QT6_DIR</b> to the Qt6 library (ex. Qt6/6.4.0/msvc2019_64),
* Set the <b>CMAKE_TOOLCHAIN_FILE</b> to the Vcpkg (ex. <vcpkg_dir>/scripts/buildsystems/vcpkg.cmake), 
* Other parameters like build type or build folder can be set for a more customized build.
#### VCPKG INSTRUCTIONS
* Clone the <b>Vcpkg</b> <code>git clone https://github.com/microsoft/vcpkg.git</code>
* Run the <i>bootstrap-vcpkg.bat</i> from cmd <code>.\vcpkg\bootstrap-vcpkg.bat</code>
* Run Vcpkg install <code>.\vcpkg\vcpkg integrate install</code>

## PACKAGE CREATION
After the build has completed the <i>scripts/create_package.sh</i> shell script should be used for creating a package. The <b>QT6_DIR</b> parameter specifies the location of the Qt6 library while the <b>OPENCV_DIR</b> parameter specifies the location of the OpenCV library. Other parameters like build type or build directory can be specified if needed.
