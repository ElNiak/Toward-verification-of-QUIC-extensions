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
cmake --version

cd $HOME/PQUIC/
#Install ivy
printf "\n\n"
printf "###### Installing Ivy:\n\n"
git clone --recurse-submodules https://github.com/ElNiak/QUIC-Ivy.git
cd QUIC-Ivy/
git checkout pquic #Jan 28 2020
mkdir $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/build
mkdir $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/temp
cd ..
bash modif.sh
rm $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/test.py
cp $HOME/PQUIC/test.py $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/


cd $HOME/PQUIC/
bash install_pquic.sh


