# Clone Ivy project 
git clone --recurse-submodules git@github.com:ElNiak/QUIC-Ivy.git --branch quic_29
mkdir QUIC-Ivy/doc/examples/quic/build
mkdir QUIC-Ivy/doc/examples/quic/test/temp

# Clone picotls project 
git clone https://github.com/h2o/picotls.git 
cd /picotls
git checkout 2464adadf28c1b924416831d24ca62380936a209 
git submodule init
git submodule update
cd /

# Clone picoquic project 
git clone https://github.com/private-octopus/picoquic.git 
cd /picoquic 
git checkout 639c9e685d37e74d357d3dd8599b9dbff90934af
cd /