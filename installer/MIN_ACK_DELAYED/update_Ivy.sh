cd $HOME/MIN_ACK_DELAYED/

#Remove Ivy
sudo rm -r $HOME/MIN_ACK_DELAYED/QUIC-Ivy/

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

