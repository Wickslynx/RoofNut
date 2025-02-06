#!/bin/sh
sudo apt update

sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxkbcommon-dev libwayland-dev 

cd install

mkdir build

cd build

cmake ..

make

sudo make install
