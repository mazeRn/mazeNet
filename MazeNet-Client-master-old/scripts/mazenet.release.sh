#!/bin/bash

# 1) Switches into the $SWE directory
# 2) Creates the cmake files for Release
# 3) Runns the make command build the project.

cd $MAZENET #1

if [ ! -d Release ]
then
#Creates the build directory if neccessary
mkdir Release

fi

cd Release
cmake -DCMAKE_BUILD_TYPE=Release $MAZENET #2
cd ..

$MAZENET/scripts/mazenet.mr.sh #3
