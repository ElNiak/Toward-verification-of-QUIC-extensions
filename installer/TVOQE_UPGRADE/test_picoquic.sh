servers=(picoquic)

tests_client=(quic_client_test
    quic_client_test_max)

tests_server=(quic_server_test_stream
    quic_server_test_max
    quic_server_test_connection_close
    quic_server_test_reset_stream)

cd $HOME/TVOQE_UPGRADE/QUIC-Ivy/
#git stash
#git pull
#git checkout quic_upgrade

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
cd $HOME/TVOQE_UPGRADE/QUIC-Ivy/doc/examples/quic/test/
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
