#!/bin/sh
IFS=$'\n'
commits=( $(git log -250 --pretty=format:"%H") )
declare -p commits

# We are (THE CHAMPION) in quic folder
# ${1} = quic version
for commit in "${commits[@]}"
do
    git checkout commit
    ivyc target=test quic_server_test_stream.ivy
    cd test
    python test.py iters=1 server=picoquic test=quic_server_test_stream stats=true >> ${1}_test_checkout.txt
    cd ..
done
