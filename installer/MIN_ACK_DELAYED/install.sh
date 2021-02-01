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

cd $HOME/MIN_ACK_DELAYED/
#Install ivy
printf "\n\n"
printf "###### Installing Ivy:\n\n"
git clone --recurse-submodules https://github.com/ElNiak/QUIC-Ivy.git
cd QUIC-Ivy/
git checkout quic_min_ack_delay #Jan 28 2020
mkdir $HOME/MIN_ACK_DELAYED/QUIC-Ivy/doc/examples/quic/build
mkdir $HOME/MIN_ACK_DELAYED/QUIC-Ivy/doc/examples/quic/test/temp
cd ..
bash modif.sh
rm $HOME/MIN_ACK_DELAYED/QUIC-Ivy/doc/examples/quic/test/test.py
cp $HOME/MIN_ACK_DELAYED/test.py $HOME/MIN_ACK_DELAYED/QUIC-Ivy/doc/examples/quic/test/

#Clone quic
printf "\n\n"
printf "###### Downloading QUIC implementations:\n\n"
mkdir quic
cd quic
[ ! -f picotls/ ] &&  git clone https://github.com/h2o/picotls.git
[ ! -f picoquic/ ] &&  git clone https://github.com/private-octopus/picoquic.git 


#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd $HOME/MIN_ACK_DELAYED/quic/picotls/
git checkout 2464adadf28c1b924416831d24ca62380936a209
git submodule init
git submodule update
cmake .
make
make check


#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd $HOME/MIN_ACK_DELAYED/quic/picoquic/
git checkout 639c9e685d37e74d357d3dd8599b9dbff90934af 
cmake .
make
./picoquic_ct

