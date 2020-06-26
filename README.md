# Toward-verification-of-QUIC-extensions

## Description of the problem
Formal methods play an important role in validating networking protocols. During the development of TLS 1.3, formal methods have helped to identify several issues with draft versions of the protocol that have been fixed before finalising the protocol. In the transport layer, the QUIC protocol has been proposed to replace the HTTP/TLS/TCP stack. This protocol is being finalised within the IETF and deployed by Google, Cloudlfare, Facebook and many others.     

A first approach to verify the correctness of the QUIC protocol has been presented last year [1]. This specification covers the basics of QUIC but many extensions are being discussed with the quic-wg [2] of the IETF and proposed by researchers [3][4]. The objective of this thesis is to explore how the specification proposed in [1] can be extended to model and verify some of the new QUIC extensions.       

This thesis could be the starting point for a PhD on using formal methods to validate networking protocols.     
[1] Formal specification and testing of QUIC   KL McMillan, LD Zuck - SIGCOMM 2019, http://mcmil.net/pubs/SIGCOMM19.pdf  
[2] https://datatracker.ietf.org/wg/quic/documents/  
[3] https://www.multipath-quic.org  
[4] https://www.pquic.org

## QUIC overview
### Sending FSM
![alt text](https://github.com/ElNiak/Toward-verification-of-QUIC-extensions/blob/master/rapport/sentFSM.PNG)

### Receiving FSM
![alt text](https://github.com/ElNiak/Toward-verification-of-QUIC-extensions/blob/master/rapport/rcvdFSM.PNG)

## Dependancies
### Ivy - Microsoft
[Github link](https://github.com/microsoft/ivy/tree/master/doc/examples/quic)

## How to

## Useful links
[Overleaf link](https://www.overleaf.com/4756785148nycvgbzrpcrb)  
[Trello link](https://trello.com/invite/b/umxKNP0a/a23a28a91982965e8f4071172df443dc/toward-verification-of-quic-extensions)
