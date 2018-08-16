#!/bin/bash

# cd glsdk_0_5_2
# premake4 gmake
# cd ..
premake4 gmake

cd glsdk_0_5_2
# make clean
make
cd ..
# make clean
make && bin/TAG_TestD