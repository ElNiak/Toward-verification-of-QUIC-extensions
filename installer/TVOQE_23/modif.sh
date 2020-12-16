#!/bin/sh

sudo rm -r /usr/local/lib/python2.7/dist-packages && sudo mkdir /usr/local/lib/python2.7/dist-packages
cd QUIC-Ivy/
#sudo pip install ms-ivy==0.1
python build_submodules.py
#sudo python setup.py install #missing python z3 module
sudo pip install ms-ivy
