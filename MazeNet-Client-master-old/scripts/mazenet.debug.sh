#!/bin/bash

# 1) Switches into the $MAZENET directory
# 2) Creates the cmake files for Debug
# 3) Runns the make command build the project.

cd $MAZENET #1

if [ ! -d Debug ]
then
#Creates the build directory if neccessary
mkdir Debug

fi

cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug $MAZENET #2
cd ..

$MAZENET/scripts/mazenet.md.sh #3
