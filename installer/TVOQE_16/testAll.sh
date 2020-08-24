#!/bin/bash

servers=(picoquic 
         quant 
         chromium
         winquic
         minq)

tests_client=()

tests_server=(quic_server_test_stream 
              quic_server_test_max 
              quic_server_test_connection_close
              quic_server_test_reset_stream)

mkdir /tmp/quic-data
cd /tmp/quic-data
wget -p --save-headers https://www.example.org

cd /home/chris/TVOQE_16/quic/chromium/src/
cd net/tools/quic/certs
./generate-certs.sh
cd -

cd /home/chris/TVOQE_16
rm ivy/doc/examples/quic/test/test.py
cp test.py ivy/doc/examples/quic/test/
cd ivy/doc/examples/quic/

export PATH=$PATH:/usr/local/go/bin
export GOROOT=/usr/local/go
export GOPATH=/home/chris/TVOQE_16/quic/go
export PATH=$PATH:$GOROOT/bin:$GOPATH/bin

printf "BUILDING TEST \n" 
for j in "${tests_client[@]}"; do : 
	ivyc target=test $j.ivy
	printf "\n"
done
for j in "${tests_server[@]}"; do : 
	ivyc target=test $j.ivy
	printf "\n"
done


printf "\n"
cd test/
printf "TEST SERVER \n" 
for j in "${tests_server[@]}"; do : 
    printf "Server => $j  " 
    for i in "${servers[@]}"; do : 
       printf "\n\nTesting => $i \n" 
       python test.py iters=1 server=$i test=$j
    done
done

printf "TEST CLIENT \n" 
for j in "${tests_client[@]}"; do : 
    printf "Client => $j  " 
    for i in "${servers[@]}"; do : 
       printf "\n\nTesting => $i \n" 
       python test.py iters=1 client=$i test=$j
    done
done
