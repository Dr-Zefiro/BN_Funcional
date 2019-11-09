#!/bin/bash

mkdir ./lib
cd ./lib || exit

#### BOST ####
git clone --recursive https://github.com/boostorg/boost.git
cd boost || exit
git submodule update
./bootstrap.sh && ./b2 && ./b2 headers
cd ..

#### TINYXML2 ####
git clone https://github.com/leethomason/tinyxml2.git