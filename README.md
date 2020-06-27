# Toward-verification-of-QUIC-extensions

## Description of the problem
Formal methods play an important role in validating networking protocols. During the development of TLS 1.3, formal methods have helped to identify several issues with draft versions of the protocol that have been fixed before finalising the protocol. In the transport layer, the QUIC protocol has been proposed to replace the HTTP/TLS/TCP stack. This protocol is being finalised within the IETF and deployed by Google, Cloudlfare, Facebook and many others.     

A first approach to verify the correctness of the QUIC protocol has been presented last year [1]. This specification covers the basics of QUIC but many extensions are being discussed with the quic-wg [2] of the IETF and proposed by researchers [3][4]. The objective of this thesis is to explore how the specification proposed in [1] can be extended to model and verify some of the new QUIC extensions.       

This thesis could be the starting point for a PhD on using formal methods to validate networking protocols.     
[1] Formal specification and testing of QUIC   KL McMillan, LD Zuck - SIGCOMM 2019, http://mcmil.net/pubs/SIGCOMM19.pdf  
[2] https://datatracker.ietf.org/wg/quic/documents/  
[3] https://www.multipath-quic.org  
[4] https://www.pquic.org

## How to
Source: http://microsoft.github.io/ivy/install.html
### Installation - Already done here
This installs Ivy into your home directory, so you don’t need sudo. In fact, **be careful *not* to use sudo when installing in your home directory**, as the files will be owned by root. Also put the first command in your .profile script, so Python will find Ivy in the future.
```shell
#Prerequisites
sudo apt-get install python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix pkg-config libssl-dev

# Install IVy
#Get the source like this:
git clone --recurse-submodules https://github.com/ElNiak/ivy.git
cd ivy

## Build the submodules like this (it takes a while):
python build_submodules.py

## Install into your local Python like this:
sudo python setup.py install

## If you want to run from the source tree for development purposes, do this instead:
export PYTHONPATH=~/lib/python2.7/site-packages:$PYTHONPATH
python setup.py develop --prefix=~

# [Optionally, build the experimental Ivy v2.0 compiler:]
python build_v2_compiler.py
```

(**Easiest way**) For Binary releases, do with sudo
```shell
 sudo apt-get install python python-pip g++ python-ply python-pygraphviz python-tk tix libssl-dev
 sudo pip install ms-ivy
```
Then clone the repository to have some examples and other interesting stuffs. ** After that proceed to the installation in https://github.com/microsoft/ivy/tree/master/doc/examples/quic **
### Usage
Run Ivy on an example, like this:
```shell
cd doc/examples
ivy client_server_example.ivy
```
Or, if you only want to use Ivy on the command line, test it like this:
```shell
ivy_check trace=true doc/examples/client_server_example_new.ivy
```
Ivy should print out a counterexample trace.
## QUIC overview
### Sending FSM
![alt text](https://github.com/ElNiak/Toward-verification-of-QUIC-extensions/blob/master/rapport/sentFSM.PNG)

### Receiving FSM
![alt text](https://github.com/ElNiak/Toward-verification-of-QUIC-extensions/blob/master/rapport/rcvdFSM.PNG)

## Dependancies
### Ivy - Microsoft
[Github link](https://github.com/microsoft/ivy/tree/master/doc/examples/quic)

## Useful links
[Overleaf link](https://www.overleaf.com/4756785148nycvgbzrpcrb)  
[Trello link](https://trello.com/invite/b/umxKNP0a/a23a28a91982965e8f4071172df443dc/toward-verification-of-quic-extensions)  
[BibTex online](https://www.bibme.org/bibtex)  
[SciHub for free paper](https://sci-hub.tw/)

## Historic of the project
1. `25-06-2020 - /` : read paper and documentation about the project. 
2. `25-06-2020 - /` : reproduce paper with draft 29 instead of 18 as in the paper and compare result
