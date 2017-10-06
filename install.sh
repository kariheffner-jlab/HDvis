#!/bin/bash

mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE= -G "CodeBlocks - Unix Makefiles" ../
cmake --build ./ --target hdvis -- -j 4
echo "HDvis built"