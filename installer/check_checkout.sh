#!/bin/sh
#sudo apt-get install xterm
IFS=$'\n'
commits=( $(git log -250 --pretty=format:"%H") )
declare -p commits

# We are (THE CHAMPION) in quic folder
# ${1} = quic version
count=1
for commit in "${commits[@]}"
do
    git stash
    rm $HOME/TVOQE_23/ivy/doc/examples/quic/test/test.py
    cp $HOME/TVOQE_23/test.py $HOME/TVOQE_23/ivy/doc/examples/quic/test/
    git checkout $commit
    ivyc target=test quic_server_test_stream.ivy
    cd test
    echo "Test ${commit} " >> ${1}_test_checkout.txt
    sudo wireshark -i lo -f quic -w quic_${1}_$count.pcap && (python test.py iters=1 server=picoquic test=quic_server_test_stream stats=true >> ${1}_test_checkout.txt)  
    cd ..
    count=count+1
    pkill wireshark
done
