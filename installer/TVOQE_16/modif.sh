#!/bin/sh

sudo rm -r /usr/local/lib/python2.7/dist-packages && sudo mkdir /usr/local/lib/python2.7/dist-packages

#(from versions: 0.1, 0.2, 0.3, 0.4, 1.7.0)
#sudo pip install ms-QUIC-Ivy==0.2
python build_submodules.py

rm QUIC-Ivy/setup.py
cp setup.py QUIC-Ivy/

cd QUIC-Ivy/
#sudo python setup.py install #missing python z3 module
#sudo pip install z3

#/usr/local/lib/python2.7/dist-packages/ms_ivy-1.7.0-py2.7.egg
sudo pip install ms-ivy
