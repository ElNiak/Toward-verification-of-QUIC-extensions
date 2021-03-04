cd quic/
[ ! -f quant/ ] && git clone https://github.com/NTAP/quant.git --branch 29


printf "\n\n"
printf "###### Installing PQUIC \n\n"
cd $HOME/TVOQE_UPGRADE_27/quic/quant/
git submodule update --init --recursive
mkdir Debug 
cd Debug
cmake ..
make

