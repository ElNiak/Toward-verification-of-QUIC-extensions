servers=(picoquic)

tests_client=(quic_client_test_stream 
              quic_client_test_max)

tests_server=(quic_server_test_stream 
              quic_server_test_max 
              quic_server_test_connection_close
              quic_server_test_reset_stream)

cd $HOME/TVOQE_Perso/QUIC-Ivy/
git pull

cd $HOME/TVOQE_Perso/

bash install_ivy.sh

rm $HOME/TVOQE_Perso/QUIC-Ivy/doc/examples/quic/test/test.py
cp $HOME/TVOQE_Perso/test.py $HOME/TVOQE_Perso/QUIC-Ivy/doc/examples/quic/test/
cd $HOME/TVOQE_Perso/QUIC-Ivy/doc/examples/quic/quic_tests

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
cd ../test/
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

cd $HOME/TVOQE_Perso/
bash remove_ivy.sh
