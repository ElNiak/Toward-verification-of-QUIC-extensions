servers=(picoquic)

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
	      quic_client_test_tp_prefadd_error)


cd $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/
#git stash
#git pull
#git checkout quic_upgrade

cd $HOME/TVOQE_UPGRADE_27/

bash install_ivy.sh

rm $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/test/test.py
cp $HOME/TVOQE_UPGRADE_27/test.py $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/test/
cd $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/quic_tests

printf "BUILDING TEST \n"
for j in "${tests_client[@]}"; do
    :
    ivyc target=test $j.ivy
    cp $j $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/build/
    cp $j.cpp $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/build/
    cp $j.h $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/build/
    rm $j
    rm $j.cpp
    rm $j.h
    printf "\n"
done

printf "\n"
cd $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/test/
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


cd $HOME/TVOQE_UPGRADE_27/
bash remove_ivy.sh
