#!/bin/bash

: "${QTDIR_FOLDER:=""}"
: "${CMAKE_BUILD_TYPE:="RelWithDebInfo"}"
: "${BUILD_FOLDER:="build"}"
: "${PACKAGE_FOLDER:="package"}"



if [[ "$QTDIR_FOLDER" == "" ]]; then
	echo "QTDIR_FOLDER not set!"
	exit 1
fi

mkdir -p $PACKAGE_FOLDER

if [[ "$CMAKE_BUILD_TYPE" == "Release" ]]; then
	DLL_POSTFIX=".dll"
else
	DLL_POSTFIX="d.dll"
fi


cp -a "${BUILD_FOLDER}/${CMAKE_BUILD_TYPE}/FaceRecon.exe" "$PACKAGE_FOLDER"
cp -a "${QTDIR_FOLDER}/bin/Qt6Core${DLL_POSTFIX}" "$PACKAGE_FOLDER"
cp -a "${QTDIR_FOLDER}/bin/Qt6Gui${DLL_POSTFIX}" "$PACKAGE_FOLDER"
cp -a "${QTDIR_FOLDER}/bin/Qt6Widgets${DLL_POSTFIX}" "$PACKAGE_FOLDER"
cp -a "${QTDIR_FOLDER}/plugins/." "$PACKAGE_FOLDER"