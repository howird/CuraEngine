FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && \
    apt-get -y install git wget autoconf automake libtool curl make g++ unzip cmake python3 python3-dev python3-sip-dev

RUN git clone -b 4.13 https://github.com/Ultimaker/libArcus.git
RUN wget https://github.com/google/protobuf/releases/download/v3.5.0/protobuf-all-3.5.0.zip

# # install protobuf
RUN unzip protobuf-all-3.5.0.zip
WORKDIR "/protobuf-3.5.0"
RUN ./autogen.sh && ./configure && make && make install && ldconfig

# install libArcus
WORKDIR "/libArcus"
RUN mkdir build && cd build && cmake .. && make && make install

# install curaengine
COPY . /CuraEngine
WORKDIR "/CuraEngine"
RUN mkdir build && cd build && cmake .. && make