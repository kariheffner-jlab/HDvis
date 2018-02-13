#!/bin/bash

mkdir build
cd build

cmake3 -DCMAKE_BUILD_TYPE= -G "CodeBlocks - Unix Makefiles" ../
cmake3 --build ./ --target hdvis -- -j 4
echo "HDvis built"