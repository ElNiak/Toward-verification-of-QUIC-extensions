#!/bin/sh
#sudo apt-get install xterm
IFS=$'\n'
commits=( $(git log -250 --pretty=format:"%H") )
declare -p commits

# We are (THE CHAMPION) in quic folder
# ${1} = quic version
count=1
mkdir -p  $HOME/TVOQE_23/ivy/doc/examples/quic/test/pcap
for commit in "${commits[@]}"
do
    git stash
    git checkout $commit
    rm $HOME/TVOQE_23/ivy/doc/examples/quic/test/test.py
    cp $HOME/TVOQE_23/test.py $HOME/TVOQE_23/ivy/doc/examples/quic/test/
    ivyc target=test quic_server_test_stream.ivy

    cd test

    echo "Test ${commit} " >> ${1}_test_checkout.txt
    touch $HOME/TVOQE_23/ivy/doc/examples/quic/test/pcap/quic_${1}_$count.pcap
    sudo chmod o=xw $HOME/TVOQE_23/ivy/doc/examples/quic/test/pcap/quic_${1}_$count.pcap

    sudo wireshark -i lo -w $HOME/TVOQE_23/ivy/doc/examples/quic/test/pcap/quic_${1}_$count.pcap -k &
    python test.py iters=1 server=picoquic test=quic_server_test_stream stats=true >> ${1}_test_checkout.txt 
    cd ..
    count=$((count+1))
    sudo pkill wireshark
done
