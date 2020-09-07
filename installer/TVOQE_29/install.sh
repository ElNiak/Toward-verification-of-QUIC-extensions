#install deps
printf "###### Installing dependencies:\n\n"
sudo apt-get install python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix gperf pkg-config libssl-dev
pip install pexpect
pip install gperf
sudo apt-get install doxygen
sudo apt-get install pkg-config
sudo apt-get install faketime libscope-guard-perl libtest-tcp-perl
sudo apt-get install libbrotli-dev
sudo apt install libev-dev libssl-dev libhttp-parser-dev libbsd-dev doxygen gperf


sudo apt remove cmake
sudo snap install cmake --classic

#sudo cp cmake-3.12.4-Linux-x86_64.sh /opt/
#cd /opt/
#sudo chmod +x /opt/cmake-3.12.4-Linux-x86_64.sh
#sudo bash /opt/cmake-3.12.4-Linux-x86_64.sh
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/cmake /usr/local/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/ccmake /usr/local/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/cmake-gui /usr/local/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/cpack /usr/local/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/ctest /usr/local/bin

#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/cmake /usr/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/ccmake /usr/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/cmake-gui /usr/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/cpack /usr/bin
#sudo ln -s /opt/cmake-3.12.4-Linux-x86_64.sh/bin/ctest /usr/bin

cmake --version

cd /home/chris/TVOQE_29/
#Install ivy
printf "\n\n"
printf "###### Installing Ivy:\n\n"
git clone --recurse-submodules https://github.com/Microsoft/ivy.git
cd ivy/
git checkout 752be924254284ded395c95f1dbd86255de6a057 #Jan 28 2020
mkdir doc/examples/quic/build
mkdir doc/examples/quic/test/temp
cd ..
bash modif.sh
rm ivy/doc/examples/quic/test/test.py
cp test.py ivy/doc/examples/quic/test/


#Clone quic
printf "\n\n"
printf "###### Downloading QUIC implementations:\n\n"
mkdir quic
cd quic
[ ! -f picotls/ ] &&  git clone https://github.com/h2o/picotls.git
[ ! -f picoquic/ ] &&  git clone https://github.com/private-octopus/picoquic.git 
[ ! -f quant/ ] &&  git clone https://github.com/NTAP/quant.git
[ ! -f go1.15.linux-amd64.tar.gz ] &&  wget https://golang.org/dl/go1.15.linux-amd64.tar.gz
mkdir go


#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd picotls/
git checkout a1769991c69e4f9b8e3d19db5cce745aaa86b271
git submodule init
git submodule update
cmake .
make
make check


#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd ../picoquic/
git checkout db015b81f3cd41abfbda58ee89e5a9de042c60b5 
cmake .
make
./picoquic_ct


#Install quant
#TODO: still some error during installation (missing package)
printf "\n\n"
printf "###### Installing Quant:\n\n"
cd ../quant/
git checkout 7f5030bf27be67032d6196812be0fab78bef8718
git submodule update --init --recursive
mkdir Debug 
cd Debug
cmake ..
make


#Install go 
printf "\n\n"
printf "###### Installing Golang:\n\n"
cd /home/chris/TVOQE_29/quic/ #TODO
sudo tar -C /usr/local -xzf go1.15.linux-amd64.tar.gz
export PATH=$PATH:/usr/local/go/bin
echo export PATH=$PATH:/usr/local/go/bin >> ~/.profile 
cd go/


#install minquic
printf "\n\n"
printf "###### Installing MinQUIC:\n\n"
mkdir src
export GOPATH=`pwd`
echo export GOPATH=`pwd` >> ~/.profile 
export GOROOT=/usr/local/go
export GOPATH=/home/chris/TVOQE_29/quic/go #TODO
export PATH=$PATH:$GOROOT/bin:$GOPATH/bin
cd src/
go get github.com/ekr/minq
go get github.com/cloudflare/cfssl/helpers
cd github.com/bifurcation/mint
git remote add ekr https://github.com/ekr/mint
git fetch ekr
git checkout ekr/quic_record_layer
cd ../../ekr/minq
go test


#Install chromium
#cd /home/chris/TVOQE_18/
#bash installChromium.sh 
