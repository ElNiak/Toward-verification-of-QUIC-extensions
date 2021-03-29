 
cd $HOME/TVOQE_UPGRADE_27/quic/
git clone --recursive https://github.com/quinn-rs/quinn.git 
cd /quinn
git checkout 0.7.0

# install RUST
curl https://sh.rustup.rs -sSf | sh

cd quinn/
cargo build --examples
cargo test
