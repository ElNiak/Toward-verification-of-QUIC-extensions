#!/bin/bash

cd /quic

#Install picotls
printf "\n\n"
printf "###### Installing PicoTLS:\n\n"
cd /quic/picotls/
cmake .
make
make check

#Install picoquic
printf "\n\n"
printf "###### Installing PicoQUIC:\n\n"
cd /quic/picoquic/
cmake .
make
./picoquic_ct

cd /
echo "Update Includes"
bash update_include.sh