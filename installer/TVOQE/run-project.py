#!/usr/bin/env python

from distutils.dir_util import copy_tree
from shutil import copyfile
import getopt
import sys
import os
from os import listdir
from os.path import isfile, join

def usage():
    message = "Usage: ./run-project -b[uild] <true/false> -m[ode] <client/server/all> -p <path/>"
    print(message)

def main(argv):     
    build = False
    mode  = "all" 
    path = ""
    
    try:                                
        opts, args = getopt.getopt(argv, "hg:b:m:p", ["help", "build=","mode=","path="])
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
            if not arg == "all" and not arg == "client" and not arg == "server":
                usage()                     
                sys.exit()            
            mode = arg  
        elif opt == '-p':
            if args[0] == "" :
                usage()                     
                sys.exit()            
            path = args[0]  
            if not path[-1] == "/" and not path[-1] == "\\":
                path += "/"

    if build:
        os.system('docker build -t quic-ivy-uclouvain .')

    if mode == "all":
        os.system('docker run -it -v '+ path + 'results:/results quic-ivy-uclouvain bash test_all.sh')
    elif mode == "client":
        os.system('docker run -it -v '+ path + 'results:/results quic-ivy-uclouvain bash test_client.sh')
    elif mode == "server":
        os.system('docker run -it -v '+ path + 'results:/results quic-ivy-uclouvain bash test_server.sh')

if __name__ == "__main__":
    main(sys.argv[1:])
