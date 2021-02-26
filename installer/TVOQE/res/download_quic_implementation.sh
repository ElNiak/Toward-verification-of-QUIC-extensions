#!/bin/bash

cd /
# Clone Ivy project 
git clone --recurse-submodules git@github.com:ElNiak/QUIC-Ivy.git --branch quic_29
mkdir QUIC-Ivy/doc/examples/quic/build
mkdir QUIC-Ivy/doc/examples/quic/test/temp

# Clone picotls project 
git clone https://github.com/h2o/picotls.git 
cd /picotls
git checkout 2464adadf28c1b924416831d24ca62380936a209 
# git submodule init
# git submodule update

# Clone picoquic project 
cd /
git clone https://github.com/private-octopus/picoquic.git 
cd /picoquic 
git checkout ad23e6c3593bd987dcd8d74fc9f528f2676fedf4 
# 639c9e685d37e74d357d3dd8599b9dbff90934af 800a1e70eda6352457bfc745e97f9ce3b7958644

#Clone quiche project
cd /
git clone --recursive https://github.com/cloudflare/quiche
cd /quiche
git checkout 37de4c81b0c5e4b626e64aee5a9f15198343eb67

#Clone quic-go project
cd /
#Install go
wget https://dl.google.com/go/go1.14.linux-amd64.tar.gz  &> /dev/null
tar xfz go1.14.linux-amd64.tar.gz &> /dev/null
rm go1.14.linux-amd64.tar.gz
#Install project
git clone https://github.com/lucas-clemente/quic-go
cd /quic-go
git checkout v0.18.1
export PATH="/go/bin:${PATH}"

#Clone AIOQuic
cd /
git clone https://github.com/aiortc/aioquic.git
cd /aioquic
git checkout 1ff7f88252584a4e6ff3d320ec239016e69c4309

#Clone mvfst
cd /
git clone https://github.com/facebookincubator/mvfst
mv tls-keys-patch.diff /mvfst
cd /mvfst
git checkout 36111c1
git apply tls-keys-patch.diff