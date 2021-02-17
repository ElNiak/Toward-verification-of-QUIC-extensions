servers=(pquic)

tests_client=(quic_client_test_max)


cd $HOME/PQUIC/QUIC-Ivy/
#git stash
#git pull
#git checkout quic_upgrade

cd $HOME/PQUIC/

bash install_ivy.sh

rm $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/test.py
cp $HOME/PQUIC/test.py $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/
cd $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/quic_tests

printf "BUILDING TEST \n"
for j in "${tests_client[@]}"; do
    :
    ivyc target=test $j.ivy
    cp $j $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/build/
    cp $j.cpp $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/build/
    cp $j.h $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/build/
    rm $j
    rm $j.cpp
    rm $j.h
    printf "\n"
done

printf "\n"
cd $HOME/PQUIC/QUIC-Ivy/doc/examples/quic/test/
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


cd $HOME/PQUIC/
bash remove_ivy.sh
