FROM ubuntu:18.10

RUN apt-get update \
 && apt-get install -qq --no-install-recommends \
    build-essential \
    cmake \
    libglfw3-dev \
    libglu1-mesa-dev \
    libx11-dev \
    libxcursor-dev \
    libxinerama-dev \
    libxi-dev \
    libxrandr-dev \
#    libxxf86vm-dev \
    mesa-common-dev \
    premake4 \
    wget \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#RUN GLFW_VERSION="3.2.1" \
# && mkdir /tmp/glfw \
# && cd /tmp/glfw \
# && wget -nv --no-check-certificate https://github.com/glfw/glfw/archive/"$GLFW_VERSION".tar.gz \
# && tar -xzf "$GLFW_VERSION".tar.gz \
# && rm "$GLFW_VERSION".tar.gz \
# && cd glfw-"$GLFW_VERSION" \
# && cmake . \
# && make install

ENTRYPOINT cd /src && ./build_script.sh 
#CMD bash
