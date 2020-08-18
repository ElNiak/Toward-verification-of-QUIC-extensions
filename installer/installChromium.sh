cd /home/chris/TVOQE_18/quic/

#Install depot_tools
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH="$PATH:/home/chris/TVOQE_18/quic/depot_tools"
echo export PATH="$PATH:/home/chris/TVOQE_18/quic/depot_tools" >> ~/.profile 

#Install chromium
mkdir chromium
cd chromium
fetch --nohooks chromium
cd src
./build/install-build-deps.sh
gclient runhooks
gn gen out/Default
autoninja -C out/Default chrome
