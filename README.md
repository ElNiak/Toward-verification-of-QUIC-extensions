# Toward-verification-of-QUIC-extensions

## Description of the problem
Formal methods play an important role in validating networking protocols. During the development of TLS 1.3, formal methods have helped to identify several issues with draft versions of the protocol that have been fixed before finalising the protocol. In the transport layer, the QUIC protocol has been proposed to replace the HTTP/TLS/TCP stack. This protocol is being finalised within the IETF and deployed by Google, Cloudlfare, Facebook and many others.     

A first approach to verify the correctness of the QUIC protocol has been presented last year [1]. This specification covers the basics of QUIC but many extensions are being discussed with the quic-wg [2] of the IETF and proposed by researchers [3][4]. The objective of this thesis is to explore how the specification proposed in [1] can be extended to model and verify some of the new QUIC extensions.       

This thesis could be the starting point for a PhD on using formal methods to validate networking protocols.     
[1] Formal specification and testing of QUIC   KL McMillan, LD Zuck - SIGCOMM 2019, http://mcmil.net/pubs/SIGCOMM19.pdf  
[2] https://datatracker.ietf.org/wg/quic/documents/  
[3] https://www.multipath-quic.org  
[4] https://www.pquic.org

## How to Ivy
Source: http://microsoft.github.io/ivy/install.html and https://github.com/microsoft/ivy/tree/master/doc/examples/quic 
### Installation

Go in `/installer` folder.

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

### Run all the test easily

Go in `/installer/TVOQE_X` and run `bash /installer/TVOQE_X/testAll.sh`


## How to eBPF (May be for futur - TODO)

Source: http://www.brendangregg.com/blog/2019-01-01/learn-ebpf-tracing.html?fbclid=IwAR1ntolwzJwZFXCIVgYEoqLpI2udHS2ArflL6RQyi2watwbQ-kL2psvCZdQ




## QUIC overview
### Sending FSM
![alt text](https://github.com/ElNiak/Toward-verification-of-QUIC-extensions/blob/master/rapport/sentFSM.PNG)

### Receiving FSM
![alt text](https://github.com/ElNiak/Toward-verification-of-QUIC-extensions/blob/master/rapport/rcvdFSM.PNG)

## Dependancies

[OpenSSL](https://wiki.openssl.org/index.php/Command_Line_Utilities) 
[Picoquic](https://github.com/private-octopus/picoquic) 

### Ivy - Microsoft
[Github link](https://github.com/microsoft/ivy/tree/master/doc/examples/quic)

## Useful links
[Overleaf link](https://www.overleaf.com/4756785148nycvgbzrpcrb)  
[BibTex online](https://www.bibme.org/bibtex)  
[SciHub for free paper](https://sci-hub.tw/)
[GoogleSheet](https://docs.google.com/spreadsheets/d/1WkqKCKSSSM3QD5_SshoD6J30v8t9DslmmHi1xmDJcsU/edit?skip_itp2_check=true&pli=1#gid=0)

## Historic of the project

[Trello link](https://trello.com/invite/b/umxKNP0a/a23a28a91982965e8f4071172df443dc/toward-verification-of-quic-extensions)  
