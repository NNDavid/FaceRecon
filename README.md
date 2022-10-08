# FaceRecon

## BUILD
### REQUIREMENTS
* MSVC 2019 compiler
* Qt6

### INSTRUCTIONS
The <i>scripts/build.sh</i> shell script should be used for building this project. The <b>CMAKE_PREFIX_PATH</b> option should be set to the Qt6 library (ex. Qt6/6.4.0/msvc2019_64). Additionally other parameters like build type or build folder can be set for a more customized build.

## PACKAGE CREATION
After the build has completed the <i>scripts/create_package.sh</i> shell script should be used for creating a package. The <b>QTDIR_FOLDER</b> parameter specifies the location of the Qt6 library. Other parameters like build type or build directory can be specified if needed.
