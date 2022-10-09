#!/bin/bash

: "${QT6_DIR:=""}"
: "${OPENCV_DIR:=""}"
: "${CMAKE_BUILD_TYPE:="RelWithDebInfo"}"
: "${BUILD_FOLDER:="build"}"
: "${PACKAGE_FOLDER:="package"}"


if [[ "$QT6_DIR" == "" ]]; then
	echo "Qt6 directory not specified!"
	exit 1
fi

if [[ "$OPENCV_DIR" == "" ]]; then
	echo "OpenCV directory not specified!"
    exit 1
fi

mkdir -p $PACKAGE_FOLDER

if [[ "$CMAKE_BUILD_TYPE" == "Release" ]]; then
	DLL_POSTFIX=".dll"
else
	DLL_POSTFIX="d.dll"
fi


cp -a "${BUILD_FOLDER}/${CMAKE_BUILD_TYPE}/FaceRecon.exe" "$PACKAGE_FOLDER"
cp -a "${QT6_DIR}/bin/Qt6Core${DLL_POSTFIX}" "$PACKAGE_FOLDER"
cp -a "${QT6_DIR}/bin/Qt6Gui${DLL_POSTFIX}" "$PACKAGE_FOLDER"
cp -a "${QT6_DIR}/bin/Qt6Widgets${DLL_POSTFIX}" "$PACKAGE_FOLDER"
cp -a "${QT6_DIR}/plugins/." "$PACKAGE_FOLDER"
cp -a "${OPENCV_DIR}/x64/vc15/bin/opencv_world460${DLL_POSTFIX}" "$PACKAGE_FOLDER"