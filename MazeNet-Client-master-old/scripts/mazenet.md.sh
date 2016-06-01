#!/bin/bash
if [ -d "$MAZENET/Debug" ]
then

cores=$(($(nproc)/2))

echo "-----------------------------"
echo ">>>building Debug client"
echo ">>>using $cores cores"
echo "-----------------------------"

cd $MAZENET/Debug
make -k -j$cores

else

echo No Path to Debug folder

fi
