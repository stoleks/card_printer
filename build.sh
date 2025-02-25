#!/bin/bash

# set-up build files
pathToMinGW="D:/msys64/mingw64/bin"
builder="$pathToMinGW/mingw32-make.exe"
cxx="$pathToMinGW/x86_64-w64-mingw32-g++.exe"
cc="$pathToMinGW/cc.exe"
gen="MinGW Makefiles"

# build configuration
cd build
cmake -G "$gen" .. \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_MAKE_PROGRAM="$builder" \
  -D CMAKE_CXX_COMPILER="$cxx" \
  -D CMAKE_C_COMPILER="$cc" \

# build with g++ compiler and 2 core
start=`date +%s`
cmake --build . -j3
end=`date +%s`
runtime=$((end-start))
runtimeMin=$((runtime / 60))
echo "It took $runtime s ($runtimeMin min) to compile project."
