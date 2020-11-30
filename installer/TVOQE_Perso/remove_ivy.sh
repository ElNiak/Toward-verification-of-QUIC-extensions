array=()
while IFS=  read -r -d $'\0'; do
    array+=("$REPLY")
done < <(find $HOME/TVOQE_Perso/QUIC-Ivy/doc/examples/quic -type f -name \*.ivy -print0 -printf "%f\n")

echo $array

SUB='test'
for j in "${array[@]}"; do : 
    # Set space as the delimiter
    IFS=' '
    #Read the split words into an array based on space delimiter
    read -a strarr <<< "$j"
    printf "Files => /usr/local/lib/python2.7/dist-packages/ivy/include/1.7/$strarr  \n" 
    if [[ ! "${strarr[0]}" == *"$SUB"* ]]; then
    	sudo rm /usr/local/lib/python2.7/dist-packages/ivy/include/1.7/${strarr[0]}
    fi
done

