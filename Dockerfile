FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install -y wget make gcc g++ build-essential libssl-dev git unzip autoconf automake libtool curl python3-dev python3-sip-dev python-is-python3 && \
    curl -sS https://bootstrap.pypa.io/get-pip.py | python3.8

RUN wget https://github.com/Kitware/CMake/releases/download/v3.20.6/cmake-3.20.6.tar.gz -O /tmp/cmake-3.20.6.tar.gz && \
    tar -zxvf /tmp/cmake-3.20.6.tar.gz  && \
    wget https://github.com/protocolbuffers/protobuf/releases/download/v3.17.1/protobuf-all-3.17.1.zip -O /tmp/protobuf-all-3.17.1.zip && \
    unzip /tmp/protobuf-all-3.17.1.zip  && \
    git clone -b 5.0 https://github.com/Ultimaker/libArcus.git

WORKDIR "/cmake-3.20.6"
RUN ./bootstrap --prefix=/usr && \
    make && \
    make install

WORKDIR "/protobuf-3.17.1"
RUN ./autogen.sh && \
    ./configure && \
    make && \
    make install && \
    ldconfig

WORKDIR "/libArcus"
RUN mkdir buil && \
    cd buil && \
    cmake -DBUILD_PYTHON=OFF .. && \
    make && \
    make install

RUN cd /home && \
    wget https://cfhcable.dl.sourceforge.net/project/polyclipping/clipper_ver6.4.2.zip -O /tmp/clipper_ver6.4.2.zip && \
    mkdir clipper6.4.2 && \
    cd clipper6.4.2 && \
    unzip /tmp/clipper_ver6.4.2.zip && \
    cd cpp && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# RUN apt-get update && \
#     apt-get install -y libstb-dev

COPY . /CuraEngine
WORKDIR "/CuraEngine"
RUN mkdir build && \
    cd build && \
    # cmake -DCMAKE_INSTALL_LIBDIR=/usr -DALLOW_IN_SOURCE_BUILD=ON -P ../cmake/StandardProjectSettings.cmake && \
    # cmake -P ../cmake/Findstb.cmake && \
    cmake -DUSE_SYSTEM_LIBS=ON .. && \
    make

# ARG CONAN_REVISIONS_ENABLED=1

# RUN cd /home && \
#     git clone -b 5.1 https://github.com/Ultimaker/CuraEngine.git && \
#     cd CuraEngine && \
#     python3 -m pip install conan && \
#     conan remote add ultimaker https://peer23peer.jfrog.io/artifactory/api/conan/cura-conan True && \
#     conan profile new default --detect && \
#     conan create . umbase/0.1.1@ultimaker/testing && \
#     conan install . --build=missing && \
#     cmake . -DCMAKE_TOOLCHAIN_FILE=cmake-build-release/conan/conan_toolchain.cmake && \
#     cmake --build .
