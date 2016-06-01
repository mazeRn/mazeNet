#!/bin/bash
if [ -d "$MAZENET/Release" ]
then

cores=$(($(nproc)/2))

echo "-----------------------------"
echo ">>>building Release client"
echo ">>>using $cores cores"
echo "-----------------------------"

cd $MAZENET/Release
make -k -j$cores

else

echo No Path to Release folder

fi
