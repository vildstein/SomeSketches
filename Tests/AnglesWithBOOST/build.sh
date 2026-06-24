#!/bin/bash

currentDir=$(pwd)
source_dir=$(pwd)
buildDir=$(pwd)/build

#cmake_build_type="-DCMAKE_BUILD_TYPE=Release"
cmake_build_type="-DCMAKE_BUILD_TYPE=Debug"


echo $currentDir
echo $cmake_build_type
echo $source_dir
echo $buildDir

cmake -G "Ninja" $cmake_build_type -S $source_dir -B $buildDir
cmake --build $buildDir
# sudo cmake --install $buildDir