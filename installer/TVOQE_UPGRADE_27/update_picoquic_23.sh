 #Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd $HOME/TVOQE_UPGRADE_27/quic/picotls/
git checkout 3fdf6a54c4c0762226afcbabda3b2016af5a8761
git submodule init
git submodule update
cmake .
make
make check


#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd $HOME/TVOQE_UPGRADE_27/quic/picoquic/
git checkout b0367d919ccd4a439c67a5e21d41ba8b5396a693 
cmake .
make
./picoquic_ct
