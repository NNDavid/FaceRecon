#!/bin/bash

: "${QT6_DIR:=""}"
: "${OPENCV_DIR:=""}"
: "${CMAKE_BUILD_TYPE:="RelWithDebInfo"}"
: "${GENERATOR:="Visual Studio 17 2022"}"
: "${MAIN_FOLDER:="$PWD"}"
: "${BUILD_FOLDER:="$MAIN_FOLDER/build"}"

if [[ "$QT6_DIR" == "" ]]; then
	echo "Qt6 directory not specified!"
    exit 1;
fi


if [[ "$OPENCV_DIR" == "" ]]; then
	echo "OpenCV directory not specified!"
    exit 1
fi

CMAKE_PREFIX_PATH="$QT6_DIR:$OPENCV_DIR"

cmake \
-DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
-DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
-B$BUILD_FOLDER \
-G"$GENERATOR" -A x64

cmake --build $BUILD_FOLDER --config $CMAKE_BUILD_TYPE