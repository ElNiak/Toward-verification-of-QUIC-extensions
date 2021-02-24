#!/bin/bash

#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd /quic/picotls/
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
curl https://sh.rustup.rs -sSf | sh
cd /quic/quiche/
cargo build --examples
cargo test

cd /
#Install go
mkdir /client /server
mkdir /logs
wget https://dl.google.com/go/go1.14.linux-amd64.tar.gz
tar xfz go1.14.linux-amd64.tar.gz
export PATH="/go/bin:${PATH}"
rm go1.14.linux-amd64.tar.gz
cd /quic/quic-go/
go build -o /client/client /client/main.go
go build -o /server/server /server/main.go

#Install NGTCP2
cd /
cd /quic/openssl
./config enable-tls1_3 --openssldir=/etc/ssl
make -j$(nproc) 
make install_sw 
cd /quic/nghttp3
autoreconf -i
./configure --enable-lib-only
make -j$(nproc)
make install-strip
cd /quic/ngtcp2
autoreconf -i
./configure \
        LIBTOOL_LDFLAGS="-static-libtool-libs" \
        LIBS="-ldl -pthread" \
        OPENSSL_LIBS="-l:libssl.a -l:libcrypto.a" \
        LIBEV_LIBS="-l:libev.a" \
        JEMALLOC_LIBS="-l:libjemalloc.a"
make -j$(nproc)
strip examples/client examples/server
cp examples/client examples/server /usr/local/bin
rm -rf /var/log/*
cd /generic-client-server
make
mkdir /logs /ngtcp2-generic
cp generic-client /ngtcp2-generic/generic-http3-client
cp generic-server /ngtcp2-generic/generic-http3-server

#Install mvfst
cd /
cd /quic/mvfst
git checkout 36111c1
git apply tls-keys-patch.diff
bash build_helper.sh
git apply samples-build-patch.diff
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


