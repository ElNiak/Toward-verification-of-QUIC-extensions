cd $HOME/TVOQE_UPGRADE_27/quic

git clone https://github.com/facebookincubator/mvfst
cp $HOME/TVOQE_UPGRADE_27/tls-keys-patch.diff $HOME/TVOQE_UPGRADE_27/quic/mvfst/
cp $HOME/TVOQE_UPGRADE_27/samples-build-patch.diff $HOME/TVOQE_UPGRADE_27/quic/mvfst/
cd mvfst
git checkout 36111c1
git apply tls-keys-patch.diff
bash build_helper.sh
git apply samples-build-patch.diff #Should not be here
cd $HOME/TVOQE_UPGRADE_27/quic/mvfst/quic/samples
cmake .
make
cd $HOME/TVOQE_UPGRADE_27/quic/mvfst/_build/build/quic/samples
make -j 8
