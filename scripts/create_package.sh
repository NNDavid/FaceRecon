#!/bin/bash

: "${QT6_DIR:=""}"
: "${CMAKE_BUILD_TYPE:="RelWithDebInfo"}"
: "${BUILD_FOLDER:="build"}"
: "${PACKAGE_FOLDER:="package"}"


if [[ "$QT6_DIR" == "" ]]; then
	echo "Qt6 directory not specified!"
	exit 1
fi


mkdir -p $PACKAGE_FOLDER
mkdir -p "${PACKAGE_FOLDER}/FaceReconDesktop"
mkdir -p "${PACKAGE_FOLDER}/FaceReconServer"

if [[ "$CMAKE_BUILD_TYPE" == "Release" ]]; then
	DLL_POSTFIX=".dll"
else
	DLL_POSTFIX="d.dll"
fi

cp -a "${BUILD_FOLDER}/FaceReconDesktop/${CMAKE_BUILD_TYPE}/." "${PACKAGE_FOLDER}/FaceReconDesktop"
cp -a "${BUILD_FOLDER}/FaceReconServer/${CMAKE_BUILD_TYPE}/." "${PACKAGE_FOLDER}/FaceReconServer"
cp -a "${QT6_DIR}/bin/Qt6Core${DLL_POSTFIX}" "${PACKAGE_FOLDER}/FaceReconDesktop"
cp -a "${QT6_DIR}/bin/Qt6Gui${DLL_POSTFIX}" "${PACKAGE_FOLDER}/FaceReconDesktop"
cp -a "${QT6_DIR}/bin/Qt6Widgets${DLL_POSTFIX}" "${PACKAGE_FOLDER}/FaceReconDesktop"
cp -a "${QT6_DIR}/plugins/." "${PACKAGE_FOLDER}/FaceReconDesktop"
cp -a "models/." "${PACKAGE_FOLDER}/FaceReconDesktop"
cp -a "web/." "${PACKAGE_FOLDER}/FaceReconServer"