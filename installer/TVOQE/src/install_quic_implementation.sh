#!/bin/bash

#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd /quic/picotls/
git submodule init
git submodule update
cmake .
make
make check

#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd /quic/picoquic/
cmake .
make
./picoquic_ct

cd /
#Install go
mkdir /logs
wget https://dl.google.com/go/go1.14.linux-amd64.tar.gz  &> /dev/null
tar xfz go1.14.linux-amd64.tar.gz &> /dev/null
export PATH="/go/bin:${PATH}"
rm go1.14.linux-amd64.tar.gz
cd /quic/quic-go/
mkdir /quic/quic-go/client /quic/quic-go/server 
go get ./...
go build -o /quic/quic-go/client/client /client/main.go
go build -o /quic/quic-go/server/server /server/main.go

#Install aioquic
cd /
cd /quic/aioquic
export PYTHONPATH=$PWD
pip3 install -e .
pip3 install aiofiles asgiref dnslib httpbin starlette wsproto

#Install Quant
cd /quic/quant/Debug
cmake ..
make

#Install mvfst
cd /
cd /quic/mvfst
bash build_helper.sh
git apply samples-build-patch.diff #Should not be here
cd /quic/mvfst/quic/samples
cmake .
make
cd /quic/mvfst/_build/build/quic/samples
make -j 8

#Update includes of python lib
cd /
echo "Update Includes"
bash update_include.sh


