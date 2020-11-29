array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find $HOME/TVOQE_Perso/QUIC-Ivy/doc/examples/quic -type f -name \*.ivy -print0 -printf "%f\n")

echo $array

for j in "${array[@]}"; do : 
    printf "Files => $j  \n" 
    sudo rm /usr/local/lib/python2.7/dist-packages/ivy/include/1.7/$j
done

