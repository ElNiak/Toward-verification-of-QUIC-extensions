cd /home/chris/TVOQE_18/quic/

#Install depot_tools
[ ! -f /home/chris/TVOQE_18/quic/depot_tools ] && git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PATH:/home/chris/TVOQE_18/quic/depot_tools"
echo export PATH="$PATH:/home/chris/TVOQE_18/quic/depot_tools" >> ~/.profile 

#Install chromium
mkdir chromium
cd chromium
[ ! -f /home/chris/TVOQE_18/quic/chromium/src ] && fetch --nohooks chromium
cd src
[ ! -f /home/chris/TVOQE_18/quic/chromium/src ] && ./build/install-build-deps.sh
[ ! -f /home/chris/TVOQE_18/quic/chromium/src ] && gclient runhooks
[ ! -f /home/chris/TVOQE_18/quic/chromium/src/out/Default ] && gn gen out/Default
#autoninja -C out/Default chrome
ninja -C out/Default quic_server quic_client
