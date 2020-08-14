#!/bin/bash

export MAKE="make"
if [ "Windows_NT" = "$OS" ];then export MAKE="mingw32-make" && export GOARCH=386; fi
export CGO_ENABLED=1

cd ./src/cgoqt
go build -buildmode=c-archive -o xmlydownloader.a
cd ../../

mkdir build
cd build
qmake CONFIG+=release ../src/xmly-downloader-qt5.pro
$MAKE -j2
cd ../
