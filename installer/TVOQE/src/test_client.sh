#!/bin/bash

#
# Launch the client suite test for each implementation
#

servers=(quinn lsquic picoquic quant quic-go aioquic)
alpn=(hq-29 hq-29 hq-29 hq-29 hq-29 hq-29)

#mvfst failed because of version negociation

tests_client=(quic_client_test_max
              quic_client_test_token_error
              quic_client_test_tp_error
              quic_client_test_double_tp_error
              quic_client_test_tp_acticoid_error
              quic_client_test_tp_limit_acticoid_error
              quic_client_test_blocked_streams_maxstream_error
              quic_client_test_retirecoid_error
              quic_client_test_newcoid_zero_error
              quic_client_test_accept_maxdata
              quic_client_test_tp_prefadd_error
              quic_client_test_no_odci
              quic_client_test_ext_min_ack_delay)

bash install_ivy.sh

rm /QUIC-Ivy/doc/examples/quic/test/test.py
cp /test.py /QUIC-Ivy/doc/examples/quic/test/
cd /QUIC-Ivy/doc/examples/quic/quic_tests

printf "BUILDING TEST \n"
for j in "${tests_client[@]}"; do
    :
    ivyc target=test $j.ivy
    cp $j /QUIC-Ivy/doc/examples/quic/build/
    cp $j.cpp /QUIC-Ivy/doc/examples/quic/build/
    cp $j.h /QUIC-Ivy/doc/examples/quic/build/
    rm $j
    rm $j.cpp
    rm $j.h
    printf "\n"
done

ITER=$1

export TEST_TYPE=client

printf "\n"
cd /QUIC-Ivy/doc/examples/quic/test/
printf "TEST CLIENT \n"
count=0
for j in "${tests_client[@]}"; do
    :
    printf "Client => $j  "
    cnt2=0
    for i in "${servers[@]}"; do
        :
        printf "\n\nTesting => $i \n"
        k=1
        until [ $k -gt $ITER ]; do
            export TEST_IMPL=$i
            export CNT=$count
            export RND=$RANDOM
            export TEST_ALPN=${alpn[cnt2]}
            printf "\n\Iteration => $k \n"
            touch /QUIC-Ivy/doc/examples/quic/test/temp/${count}_quic_client_${j}.pcap
            chmod o=xw /QUIC-Ivy/doc/examples/quic/test/temp/${count}_quic_client_${j}.pcap
            tshark -i lo -w /QUIC-Ivy/doc/examples/quic/test/temp/${count}_quic_client_${j}.pcap -f "udp" &
            python test.py iters=1 client=$i test=$j > res_client.txt 2>&1
            ((k++))
            kill $(lsof -t -i udp) >/dev/null 2>&1
            printf "\n"
            pkill tshark
            cp res_client.txt /QUIC-Ivy/doc/examples/quic/test/temp/${count}/res_client.txt
            count=$((count + 1))
        done
        cnt2=$((cnt2 + 1))
    done
done

cd /
bash remove_ivy.sh

cp -R  /QUIC-Ivy/doc/examples/quic/test/temp/ /results

cd /results
python create-csv.py