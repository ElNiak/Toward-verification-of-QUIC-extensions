#!/bin/bash

#
# Launch the server suite test for each implementation
# Remove sleep

# 

servers=(picoquic quiche mvfst aioquic quic-go)

tests_server=(quic_server_test_stream
              quic_server_test_max)

            #  quic_server_test_token_error  
            #  quic_server_test_tp_error
            #  quic_server_test_tp_acticoid_error
            #  quic_server_test_connection_close
            #  quic_server_test_reset_stream
	        #  quic_server_test_blocked_streams_maxstream_error
	        #  quic_server_test_retirecoid_error
	        #  quic_server_test_newcoid_zero_error
	        #  quic_server_test_handshake_done_error
	        #  quic_server_test_stop_sending
            #  quic_server_test_double_tp_error
	        #  quic_server_test_tp_limit_acticoid_error
	        #  quic_server_test_accept_maxdata
            #  quic_server_test_ext_min_ack_delay
            #  quic_server_test_no_icid

cd /

bash install_ivy.sh

rm /QUIC-Ivy/doc/examples/quic/test/test.py
cp /test.py /QUIC-Ivy/doc/examples/quic/test/
cd /QUIC-Ivy/doc/examples/quic/quic_tests

printf "BUILDING TEST \n"
for j in "${tests_server[@]}"; do
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

printf "\n"
cd /QUIC-Ivy/doc/examples/quic/test/
printf "TEST SERVER \n"
count=0
for j in "${tests_server[@]}"; do
    :
    printf "Server => $j  "
    for i in "${servers[@]}"; do
        :
        printf "\n\nTesting => $i \n"
        k=0
        until [ $k -gt $ITER ]; do
            printf "\n\Iteration => $k \n"
            #TODO slow down 1sec
            touch /QUIC-Ivy/doc/examples/quic/test/temp/${count}_quic_server_${j}.pcap
            chmod o=xw /QUIC-Ivy/doc/examples/quic/test/temp/${count}_quic_server_${j}.pcap
            tshark -i lo -w /QUIC-Ivy/doc/examples/quic/test/temp/${count}_quic_server_${j}.pcap -f "udp" &
            python test.py server=$i test=$j > res_server.txt 2>&1
            ((k++))
            printf "\n"
            pkill tshark
            cp res_server.txt /QUIC-Ivy/doc/examples/quic/test/temp/${count}/res_server.txt
            count=$((count + 1))
        done
	printf "\n"
    done
done

cd /
bash remove_ivy.sh

cp -R /QUIC-Ivy/doc/examples/quic/test/temp/ /results


cd /results
python create-csv.py