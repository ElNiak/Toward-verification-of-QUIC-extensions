#Prep test data from www.example.org
mkdir /tmp/quic-data
cd /tmp/quic-data
wget -p --save-headers https://www.example.org

#Generate certificates
cd $HOME/TVOQE_UPGRADE/quic/chromium/src/

#https://chromium.googlesource.com/chromium/src/+/master/docs/linux/cert_management.md
#sudo apt-get install libnss3-tools
#certutil -d sql:$HOME/.pki/nssdb -L

cd net/tools/quic/certs
sudo ./generate-certs.sh
cd -

servers=(chromium)

tests_client=(quic_client_test_stream
    quic_client_test_max)

tests_server=(quic_server_test_stream
    quic_server_test_max
    quic_server_test_connection_close
    quic_server_test_reset_stream)

cd $HOME/TVOQE_UPGRADE/QUIC-Ivy/
git stash
git pull
git checkout quic_18_upgrade_chris

cd $HOME/TVOQE_UPGRADE/

bash install_ivy.sh

rm $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/test/test.py
cp $HOME/TVOQE_UPGRADE/test.py $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/test/
cd $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/quic_tests

printf "BUILDING TEST \n"
for j in "${tests_client[@]}"; do
    :
    ivyc target=test $j.ivy
    cp $j $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/build/
    cp $j.cpp $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/build/
    cp $j.h $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/build/
    rm $j
    rm $j.cpp
    rm $j.h
    printf "\n"
done
for j in "${tests_server[@]}"; do
    :
    ivyc target=test $j.ivy
    cp $j $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/build/
    cp $j.cpp $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/build/
    cp $j.h $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/build/
    rm $j
    rm $j.cpp
    rm $j.h
    printf "\n"
done

printf "\n"
cd ../test/
printf "TEST SERVER \n"
for j in "${tests_server[@]}"; do
    :
    printf "Server => $j  "
    for i in "${servers[@]}"; do
        :
        printf "\n\nTesting => $i \n"
        python test.py iters=1 server=$i test=$j
    done
done

printf "TEST CLIENT \n"
for j in "${tests_client[@]}"; do
    :
    printf "Client => $j  "
    for i in "${servers[@]}"; do
        :
        printf "\n\nTesting => $i \n"
        python test.py iters=1 client=$i test=$j
    done
done
cd $HOME/TVOQE_UPGRADE/
bash remove_ivy.sh
