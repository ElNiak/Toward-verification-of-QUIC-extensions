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
    ssh   = ""

    try:                                
        opts, args = getopt.getopt(argv, "hg:b:m:s", ["help", "build=","mode=","ssh="])
    except getopt.GetoptError:          
        usage()                         
        sys.exit(2)
    if len(opts) < 2:
        usage()                         
        sys.exit(2)                    
    for opt, arg in opts:                
        if opt in ("-h", "--help"):      
            usage()                     
            sys.exit()                       
        elif opt == '-b' and arg.lower() == "true":                
            build = True    
        elif opt == '-m':
            if not arg == "all" and not arg == "client" and not arg == "server":
                usage()                     
                sys.exit()            
            mode = arg  
        elif opt == '-s':
            if ssh == "" or arg == "":
                usage()                     
                sys.exit()            
            ssh = arg   

    if build:
        os.system('docker build -t quic-ivy .')

    if mode == "all":
        os.system('docker run -it -v results:/results quic-ivy bash test_all.sh')
    elif mode == "client":
        os.system('docker run -it -v results:/results quic-ivy bash test_client.sh')
    elif mode == "server":
        os.system('docker run -it -v results:/results quic-ivy bash test_server.sh')

if __name__ == "__main__":
    main(sys.argv[1:])
