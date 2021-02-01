#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd $HOME/MIN_ACK_DELAYED/quic/picotls/
git checkout 4e6080b6a1ede0d3b23c72a8be73b46ecaf1a084
git submodule init
git submodule update
cmake .
make
make check


#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd $HOME/MIN_ACK_DELAYED/quic/picoquic/
git checkout 95dd82f 
cmake .
make
./picoquic_ct
