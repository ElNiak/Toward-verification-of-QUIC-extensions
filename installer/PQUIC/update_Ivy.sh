cd $HOME/PQUIC/

#Remove Ivy
sudo rm -r $HOME/PQUIC/QUIC-Ivy/

#Install ivy
printf "\n\n"
printf "###### Installing Ivy:\n\n"
git clone --recurse-submodules https://github.com/ElNiak/QUIC-Ivy.git
cd QUIC-Ivy/
git checkout quic_27_upgrade #Jan 28 2020
mkdir $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/build
mkdir $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/temp
cd ..
bash modif.sh
rm $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/test.py
cp $HOME/PQUIC/test.py $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/

