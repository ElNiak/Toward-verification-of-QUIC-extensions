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
git submodule init
git submodule update

# Clone picoquic project 
cd /
git clone https://github.com/private-octopus/picoquic.git 
cd /picoquic 
git checkout 800a1e70eda6352457bfc745e97f9ce3b7958644

#Clone quiche project
cd /
git clone --recursive https://github.com/cloudflare/quiche
cd /quiche
git checkout 37de4c81b0c5e4b626e64aee5a9f15198343eb67


#Clone quic-go project
cd /
#Install go
wget https://dl.google.com/go/go1.14.linux-amd64.tar.gz
tar xfz go1.14.linux-amd64.tar.gz
rm go1.14.linux-amd64.tar.gz
#Install project
git clone https://github.com/lucas-clemente/quic-go
cd /quic-go
git checkout v0.18.1
export PATH="/go/bin:${PATH}"
go get ./...
mkdir /client /server
mkdir /logs

#Clone NGTCP2
cd /
git clone --depth 1 -b OpenSSL_1_1_1g-quic-draft-29 https://github.com/tatsuhiro-t/openssl
git clone --branch draft-29 --depth 1 https://github.com/ngtcp2/nghttp3
git clone  --branch draft-29 --depth 1 https://github.com/ngtcp2/ngtcp2

#Clone mvfst
cd /
git clone https://github.com/facebookincubator/mvfst