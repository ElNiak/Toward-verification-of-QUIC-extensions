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

#Install quiche & RUST
cd /
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs > rustup.rs 
# curl https://sh.rustup.rs -sSf > rustup.rs 
sh rustup.rs -y 
#sudo export $HOME/.cargo/bin
cd /quic/quiche/
cargo build --examples
cargo test

cd /
#Install go
mkdir /logs
wget https://dl.google.com/go/go1.14.linux-amd64.tar.gz  &> /dev/null
tar xfz go1.14.linux-amd64.tar.gz &> /dev/null
export PATH="/go/bin:${PATH}"
rm go1.14.linux-amd64.tar.gz
cd /quic/quic-go/
go get ./...
go build -o /client/client /client/main.go
go build -o /server/server /server/main.go

#Install aioquic
cd /
cd /quic/aioquic
pip3 install -e .
pip3 install aiofiles asgiref dnslib httpbin starlette wsproto


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
mkdir /mvfst-generic
cp /quic/mvfst/_build/build/quic/samples/generic /mvfst-generic/

#Update includes of python lib
cd /
echo "Update Includes"
bash update_include.sh


