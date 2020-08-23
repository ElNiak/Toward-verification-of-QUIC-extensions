cd /home/chris/TVOQE_18/quic/ #TODO

#Install depot_tools
[ ! -f depot_tools/ ] && git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PATH:/home/chris/TVOQE_18/quic/depot_tools" #TODO
echo export PATH="$PATH:/home/chris/TVOQE_18/quic/depot_tools" >> ~/.profile 

#Install chromium
mkdir chromium
cd chromium
[ ! -f src/ ] && fetch --nohooks chromium
cd src
[ ! -f src/ ] && ./build/install-build-deps.sh
[ ! -f src/ ] && gclient runhooks
[ ! -f src/out/Default ] && gn gen out/Default
#autoninja -C out/Default chrome
ninja -C out/Default quic_server 
ninja -C out/Default quic_client
