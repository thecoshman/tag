#!/bin/bash

docker build . -t tag_build_env
# need to mount password file
# docker run -it --rm --user=`id -u`:`id -g` -v $(pwd):/src tag_build_env
docker run -it --rm -v $(pwd):/src tag_build_env
