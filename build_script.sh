#!/bin/bash

# cd glsdk_0_5_2
# premake4 gmake
# cd ..
premake4 gmake

cd glsdk_0_5_2
# make clean
# make
make config=release
cd ..
# make clean config=release
# make
make config=release && bin/TAG_Test