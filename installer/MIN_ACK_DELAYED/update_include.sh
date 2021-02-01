#sudo rm -r /usr/local/lib/python2.7/dist-packages && sudo mkdir /usr/local/lib/python2.7/dist-packages
#sudo pip install ms-ivy

array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find $HOME/MIN_ACK_DELAYED/QUIC-Ivy/ivy/include/1.7/ -type f -name \*.ivy -print0)

echo $array

#sudo rm -r /usr/local/lib/python2.7/dist-packages/ivy/include/1.7

SUB='test'
for j in "${array[@]}"; do : 
    if [[ ! "$j" == *"$SUB"* ]]; then
	printf "Files => $j  \n" 
    	sudo cp $j /usr/local/lib/python2.7/dist-packages/ivy/include/1.7
    fi
done

#cd $HOME/MIN_ACK_DELAYED/QUIC-Ivy/ivy/ TODO add manually lines in ivy to cpp
#python -m compileall ivy_to_cpp.py
#sudo cp ivy_to_cpp.py /usr/local/lib/python2.7/dist-packages/ivy/
cd /usr/local/lib/python2.7/dist-packages/ivy/
sudo python -m compileall ivy_to_cpp.py
sudo python -m compileall ivy_cpp_types.py

echo "CP picotls lib"
sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/libpicotls-core.a /usr/local/lib/python2.7/dist-packages/ivy/lib
sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/libpicotls-core.a $HOME/MIN_ACK_DELAYED/QUIC-Ivy/ivy/lib
sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/libpicotls-minicrypto.a /usr/local/lib/python2.7/dist-packages/ivy/lib
sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/libpicotls-minicrypto.a $HOME/MIN_ACK_DELAYED/QUIC-Ivy/ivy/lib
sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/libpicotls-openssl.a /usr/local/lib/python2.7/dist-packages/ivy/lib
sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/libpicotls-openssl.a $HOME/MIN_ACK_DELAYED/QUIC-Ivy/ivy/lib

sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/include/picotls.h /usr/local/lib/python2.7/dist-packages/ivy/include
sudo cp $HOME/MIN_ACK_DELAYED/quic/picotls/include/picotls.h $HOME/MIN_ACK_DELAYED/QUIC-Ivy/ivy/include
sudo cp -r $HOME/MIN_ACK_DELAYED/quic/picotls/include/picotls /usr/local/lib/python2.7/dist-packages/ivy/include
sudo cp -r $HOME/MIN_ACK_DELAYED/quic/picotls/include/picotls $HOME/MIN_ACK_DELAYED/QUIC-Ivy/ivy/include
