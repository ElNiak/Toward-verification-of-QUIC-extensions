 
cd $HOME/TVOQE_Perso/quic/
git clone --recursive https://github.com/cloudflare/quiche

# install RUST
curl https://sh.rustup.rs -sSf | sh

cd quiche/
cargo build --examples
cargo test
