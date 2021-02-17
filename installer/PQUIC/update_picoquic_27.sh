cd quic/
sudo rm -r picotls/
git clone https://github.com/private-octopus/picoquic.git

#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd $HOME/PQUIC/quic/picotls/
git checkout 2464adadf28c1b924416831d24ca62380936a209
git submodule init
git submodule update
cmake .
make
make check


#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd $HOME/PQUIC/quic/picoquic/
git checkout master #639c9e685d37e74d357d3dd8599b9dbff90934af 
cmake .
make
./picoquic_ct
