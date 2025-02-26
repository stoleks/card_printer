#!/bin/bash

usage() { echo "Usage: $0 [-h for help] [-c for configuration]" 1>&2; exit 1; }

# set-up build files
pathToMinGW="D:/msys64/mingw64/bin"
builder="$pathToMinGW/mingw32-make.exe"
cxx="$pathToMinGW/x86_64-w64-mingw32-g++.exe"
cc="$pathToMinGW/cc.exe"
gen="MinGW Makefiles"

# build configuration if asked to
while getopts ":hc" opt; do
  case ${opt} in
    c)
      mkdir -p build
      cmake -G "$gen" build \
        -D CMAKE_MAKE_PROGRAM="$builder" \
        -D CMAKE_CXX_COMPILER="$cxx" \
        -D CMAKE_C_COMPILER="$cc" \
      ;;
    h | *)
      usage
      ;;
  esac
done

# build with g++ compiler and 3 core
start=`date +%s`
cmake --build build -j3
end=`date +%s`
runtime=$((end-start))
runtimeMin=$((runtime / 60))
echo "It took $runtime s ($runtimeMin min) to compile project."
