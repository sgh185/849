#!/bin/bash

export CC=clang
export CPP=clang++
export CXX=clang++

rm -rf build/ ; 
mkdir build ; 
cd build ; 
cmake -DCMAKE_INSTALL_PREFIX="${PROJ}/passes/built_passes" -DCMAKE_BUILD_TYPE=Debug ../ ; 
make -j ;
make install ;
cd ../ 
