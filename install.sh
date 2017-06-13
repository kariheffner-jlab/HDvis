#!/bin/bash

mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE= -G "CodeBlocks - Unix Makefiles" ../
cmake --build ./ --target EveStandAlone -- -j 4
echo "EveStandAlone built"