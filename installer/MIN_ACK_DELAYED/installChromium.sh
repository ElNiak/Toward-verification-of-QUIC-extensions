cd $HOME/MIN_ACK_DELAYED/quic/

#Install depot_tools
[ ! -f $HOME/MIN_ACK_DELAYED/quic/depot_tools ] && git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PATH:$HOME/MIN_ACK_DELAYED/quic/depot_tools"
echo export PATH="$PATH:$HOME/MIN_ACK_DELAYED/quic/depot_tools" >> ~/.profile 

#Install chromium
mkdir chromium
cd chromium
[ ! -f $HOME/MIN_ACK_DELAYED/quic/chromium/src ] && fetch --nohooks chromium
cd src
[ ! -f $HOME/MIN_ACK_DELAYED/quic/chromium/src ] && ./build/install-build-deps.sh
[ ! -f $HOME/MIN_ACK_DELAYED/quic/chromium/src ] && gclient runhooks
[ ! -f $HOME/MIN_ACK_DELAYED/quic/chromium/src/out/Default ] && gn gen out/Default
#autoninja -C out/Default chrome
ninja -C out/Default quic_server
ninja -C out/Default quic_client


