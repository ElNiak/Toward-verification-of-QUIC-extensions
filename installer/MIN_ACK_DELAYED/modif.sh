#!/bin/sh

sudo rm -r /usr/local/lib/python2.7/dist-packages && sudo mkdir /usr/local/lib/python2.7/dist-packages
cd $HOME/MIN_ACK_DELAYED/QUIC-Ivy/
#sudo pip install ms-ivy==0.1
python build_submodules.py
#sudo pip uninstall ms-ivy
#sudo pip uninstall pyparsing
#pip install pyparsing==2.1.4
#sudo pip install functools32
#sudo python setup.py install #missing python z3 module
sudo pip install ms-ivy
