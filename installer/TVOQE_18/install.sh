#install deps
printf "###### Installing dependencies:\n\n"
sudo apt-get install python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix gperf pkg-config libssl-dev
pip install pexpect chardet
pip install gperf 
sudo apt-get install doxygen
sudo apt-get install mercurial
sudo apt-get install pkg-config
sudo apt-get install faketime libscope-guard-perl libtest-tcp-perl
sudo apt-get install libbrotli-dev
sudo apt install libev-dev libssl-dev libhttp-parser-dev libbsd-dev doxygen gperf

sudo apt remove cmake
sudo apt install  cmake #3.10.2

#Install ivy
printf "\n\n"
printf "###### Installing Ivy:\n\n"
git clone --recurse-submodules https://github.com/ElNiak/QUIC-Ivy.git
cd QUIC-Ivy/
git checkout quic18_client && git pull origin quic18_client
mkdir doc/examples/quic/build
mkdir doc/examples/quic/test/temp
cd ..
bash modif.sh
rm QUIC-Ivy/doc/examples/quic/test/test.py
cp test.py QUIC-Ivy/doc/examples/quic/test/


#Clone quic
printf "\n\n"
printf "###### Downloading QUIC implementations:\n\n"
mkdir quic
cd quic
[ ! -f $HOME/TVOQE_18/quic/picotls ] &&  git clone https://github.com/h2o/picotls.git
[ ! -f $HOME/TVOQE_18/quic/picoquic ] &&  git clone https://github.com/private-octopus/picoquic.git 
[ ! -f $HOME/TVOQE_18/quic/quant ] &&  git clone https://github.com/NTAP/quant.git
[ ! -f $HOME/TVOQE_18/quic/go1.15.linux-amd64.tar.gz ] &&  wget https://golang.org/dl/go1.15.linux-amd64.tar.gz
mkdir go


#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd picotls/
git checkout 4e6080b6a1ede0d3b23c72a8be73b46ecaf1a084
git submodule init
git submodule update
cmake .
make
make check


#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd ../picoquic/
git checkout 95dd82f 
cmake .
make
./picoquic_ct


#Install quant
#TODO: still some error during installation (missing package)
printf "\n\n"
printf "###### Installing Quant:\n\n"
cd ../quant/
#git checkout 18
git checkout b55051011a3a040ccc93e83add29dec46eceda54 #+- good
#git checkout 317163cd599f9c33e3d0473a338ad2ac03527f26
#git checkout 0f02be60467689878fc31135d9dbc5e1d623e902

#quic17
#git checkout 4ed3af4

git submodule update --init --recursive
mkdir Debug 
cd Debug
cmake ..
make
#mkdir Release
#cd Release
#cmake -DCMAKE_BUILD_TYPE=Release ..
#make

#Install go 
printf "\n\n"
printf "###### Installing Golang:\n\n"
cd $HOME/TVOQE_18/quic/
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
export GOPATH=$HOME/TVOQE_18/quic/go
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
#cd $HOME/TVOQE_18/
#bash installChromium.sh 
