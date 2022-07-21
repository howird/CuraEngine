FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && \
    apt-get -y install git wget autoconf automake libtool curl make g++ unzip cmake python3 python3-dev python3-sip-dev

RUN git clone -b 4.13 https://github.com/Ultimaker/libArcus.git
RUN wget https://github.com/google/protobuf/releases/download/v3.5.0/protobuf-all-3.5.0.zip -O /tmp/protobuf-all-3.5.0.zip

# # install protobuf
RUN unzip /tmp/protobuf-all-3.5.0.zip
WORKDIR "/protobuf-3.5.0"
RUN ./autogen.sh && ./configure && make && make install && ldconfig

# install libArcus
WORKDIR "/libArcus"
RUN mkdir build && cd build && cmake .. && make && make install

# install curaengine
COPY . /CuraEngine
WORKDIR "/CuraEngine"
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

# WORKDIR "/home"
# RUN ln -s /CuraEngine/build/CuraEngine /usr/bin/cura


# RUN wget https://raw.githubusercontent.com/Ultimaker/Cura/4.13/resources/definitions/fdmprinter.def.json && \
#     wget https://raw.githubusercontent.com/Ultimaker/Cura/4.13/resources/definitions/fdmextruder.def.json && \
#     wget https://raw.githubusercontent.com/Ultimaker/Cura/4.13/resources/definitions/prusa_i3.def.json && \

# RUN wget https://raw.githubusercontent.com/KrisRoofe/curaengine-dockerfile/master/herringbone-gear-large.stl && \
#     wget https://upload.wikimedia.org/wikipedia/commons/3/36/3D_model_of_a_Cube.stl

# RUN cura slice -p -j ./fdmprinter.def.json -j prusa_i3.def.json -l herringbone-gear-large.stl -o gear.gcode && \
#     cura slice -p -j ./fdmprinter.def.json -j prusa_i3.def.json -l 3D_model_of_a_Cube.stl -o cube.gcode
