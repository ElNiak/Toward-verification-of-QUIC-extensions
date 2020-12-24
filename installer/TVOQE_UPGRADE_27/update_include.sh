sudo rm -r /usr/local/lib/python2.7/dist-packages && sudo mkdir /usr/local/lib/python2.7/dist-packages
sudo pip install ms-ivy

array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/ivy/include/1.7/ -type f -name \*.ivy -print0)

echo $array

#sudo rm -r /usr/local/lib/python2.7/dist-packages/ivy/include/1.7

SUB='test'
for j in "${array[@]}"; do : 
    if [[ ! "$j" == *"$SUB"* ]]; then
	printf "Files => $j  \n" 
    	sudo cp $j /usr/local/lib/python2.7/dist-packages/ivy/include/1.7
    fi
done

cd $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/ivy/
sudo cp ivy_to_cpp.py /usr/local/lib/python2.7/dist-packages/ivy/
sudo python -m compileall /usr/local/lib/python2.7/dist-packages/ivy/ivy_to_cpp.py ivy_to_cpp.py