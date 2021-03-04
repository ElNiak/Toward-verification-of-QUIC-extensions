sudo apt install libssl-dev python3-dev

cd $HOME/TVOQE_UPGRADE_27/quic/

git clone https://github.com/aiortc/aioquic.git
cd aioquic
git checkout 0.9.3

pip3 install -e .
pip3 install aiofiles asgiref dnslib httpbin starlette wsproto
