cd $HOME/TVOQE_UPGRADE_27/quic/

git clone https://boringssl.googlesource.com/boringssl
cd boringssl

sudo apt-get update && \
    sudo apt-get install -y build-essential software-properties-common \
                       zlib1g-dev libevent-dev

git checkout a2278d4d2cabe73f6663e3299ea7808edfa306b9
cmake . &&  make
export BORINGSSL=$PWD


cd $HOME/TVOQE_UPGRADE_27/quic/
git clone https://github.com/litespeedtech/lsquic.git
cd lsquic
git submodule init
git submodule update
cmake -DBORINGSSL_DIR=$BORINGSSL .
make
make test

