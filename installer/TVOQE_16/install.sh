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

#Install ivy
printf "\n\n"
printf "###### Installing Ivy:\n\n"
git clone https://github.com/ElNiak/QUIC-Ivy.git #--recurse-submodules
cd ivy/
git checkout quic15_merge_temp
#quic18_client
#11f9cd8d7d55b4fafcd53386a5cfc99f7aba7e6b #anomaly23 quic16
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
#[ ! -f z3/ ] &&  git clone https://github.com/Z3Prover/z3.git
[ ! -f picotls/ ] &&  git clone https://github.com/h2o/picotls.git
[ ! -f picoquic/ ] &&  git clone https://github.com/private-octopus/picoquic.git 
[ ! -f quant/ ] &&  git clone https://github.com/NTAP/quant.git
[ ! -f go1.15.linux-amd64.tar.gz ] &&  wget https://golang.org/dl/go1.15.linux-amd64.tar.gz
mkdir go

#Install z3
printf "\n\n"
printf "###### Installing Z3:\n\n"
#cd z3/
#mkdir build
#cd build
#cmake -G "Ninja" ../
#ninja


#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd picotls/
git checkout 1c8daa82bed17e36226036e4a7ef347835373f89 # Dec 10, 2018
#8443c09c0f091482679e0b32c4f238928b7f5c1e #Oct 4, 2018
#502690178d9ae570da6689e27209b6569f70d035 # Aug 14, 2018
git submodule init
git submodule update
cmake .
make
make check


#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd ../picoquic/
git checkout 89a5117 
cmake .
make
./picoquic_ct


#Install quant
#TODO: still some error during installation (missing package)
printf "\n\n"
printf "###### Installing Quant:\n\n"
cd ../quant/
git checkout 4ed3af4
git submodule update --init --recursive
mkdir Debug 
cd Debug
cmake ..
make


#Install go 
printf "\n\n"
printf "###### Installing Golang:\n\n"
cd /home/chris/TVOQE_16/quic/
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
export GOPATH=/home/chris/TVOQE_16/quic/go
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
#cd /home/chris/TVOQE_16/
#bash installChromium.sh 
