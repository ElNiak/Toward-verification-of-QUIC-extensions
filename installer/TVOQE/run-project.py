#!/usr/bin/env python

from distutils.dir_util import copy_tree
from shutil import copyfile
import getopt
import sys
import os
from os import listdir
from os.path import isfile, join

def usage():
    message = "Usage: ./run-project -b <true/false> -m <client/server/all>"
    print(message)

def main(argv):     
    build = False
    mode  = "all" 

    try:                                
        opts, args = getopt.getopt(argv, "hg:bm", ["help", "build=","mode="])
    except getopt.GetoptError:          
        usage()                         
        sys.exit(2)                     
    for opt, arg in opts:                
        if opt in ("-h", "--help"):      
            usage()                     
            sys.exit()                       
        elif opt == '-b' and arg.lower() == "true":                
            build = True    
        elif opt == '-m':
            if not arg == "all" or not arg == "client" or not arg == "server":
                usage()                     
                sys.exit()            
            mode = arg   

    if build:
        os.system('docker build -t quic-ivy:quic-ivy .')

    if mode == "all":
        os.system('docker run -it --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" -v ${PWD}/results:/results -e DISPLAY='+ os.environ["DISPLAY"] +' quic-ivy:quic-ivy ./test_all.sh')
    elif mode == "client":
        os.system('docker run -it --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" -v ${PWD}/results:/results -e DISPLAY='+ os.environ["DISPLAY"] +' quic-ivy:quic-ivy ./test_client.sh')
    elif mode == "server":
        os.system('docker run -it --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" -v ${PWD}/results:/results -e DISPLAY='+ os.environ["DISPLAY"] +' quic-ivy:quic-ivy ./test_server.sh')