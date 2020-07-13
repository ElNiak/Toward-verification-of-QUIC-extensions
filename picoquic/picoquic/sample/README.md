picoquic sample
===============

The sample program is a simple QUIC client/server demo.

Building
--------
picoquic\_sample is built as part of the compilation process of picoquic. It
will be available in the root folder.

Usage
-----
Usage:
    ../picoquic_sample client server_name port folder *queried_file
or :
    ../picoquic_sample server port cert_file private_key_file folder

Example
-------

Generate the certificates:
```
openssl req -x509 -newkey rsa:2048 -days 365 -keyout ca-key.pem -out ca-cert.pem
openssl req -newkey rsa:2048 -keyout server-key.pem -out server-req.pem
```
These commands will prompt a few questions, you don't need to put actual data
for this simple test.

Create a folder to hold server files:
```
mkdir server_files
echo "Hello world!" >> ./server_files/index.htm
```
And run the server:
```
./picoquic_sample server 4433 ./ca-cert.pem ./server-key.pem ./server_files
```
Then, test if you can reach it using the client:
```
./picoquic_sample client localhost 4433 /tmp index.htm
```

Getting logs
------------
Both server and clients will create logs of the connections if they can write files
in the expected folders. If you want logs, you will need to create these
folders before launching the server or the client.

For the client, create a folder with the name `sample_client_log`:
```
mkdir sample_client_log
```
For the server, create a folder with the name `sample_server_log`:
```
mkdir sample_server_log
```
Client and server log files are binary files, with names derived from
the identifiers of the connections. They will have names like
`012345678abcdef.client.log` or `012345678abcdef.server.log`. 
They can be used to generate files in `qlog` format, which are
text files encoding the connection events according to the JSON
syntax. These files can be explored with a variety of tools,
such as [qvis](https://qvis.edm.uhasselt.be/). To obtain the `qlog`
from the binary log file, use the program `picolog_t`:
```
picolog_t -f qlog -o "." 012345678abcdef.client.log
```