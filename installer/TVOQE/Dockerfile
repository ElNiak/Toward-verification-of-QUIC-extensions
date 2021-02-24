FROM ubuntu:18.04 as intermediate

# https://vsupalov.com/build-docker-image-clone-private-repo-ssh-key/
# install git
RUN apt-get update
RUN apt-get install -y git wget tar 

# add credentials on build TODO
ARG SSH_PRIVATE_KEY
RUN mkdir /root/.ssh/
COPY .ssh/id_rsa /root/.ssh/id_rsa
RUN chmod 400 /root/.ssh/id_rsa

# make sure your domain is accepted
RUN touch /root/.ssh/known_hosts
RUN ssh-keyscan github.com >> /root/.ssh/known_hosts

ADD res/download_quic_implementation.sh download_quic_implementation.sh 

RUN bash download_quic_implementation.sh

FROM ubuntu:18.04

# Install dependencies

RUN apt-get update
RUN apt-get install -y apt-utils

RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata curl wget tar 
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix gperf pkg-config libssl-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y doxygen pkg-config faketime libscope-guard-perl libtest-tcp-perl libbrotli-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libev-dev libssl-dev libhttp-parser-dev libbsd-dev snapd
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y cmake wireshark tshark
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y binutils autoconf automake autotools-dev libtool pkg-config libev-dev libjemalloc-dev ca-certificates mime-suppor
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libboost-all-dev libevent-dev libdouble-conversion-dev libgoogle-glog-dev libgflags-dev libiberty-dev liblz4-dev liblzma-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libsnappy-dev zlib1g-dev binutils-dev libjemalloc-dev libsodium-dev sudo
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y git
RUN pip install pexpect chardet 
RUN pip install gperf pandas scandir

# Install Ivy

COPY --from=intermediate QUIC-Ivy /QUIC-Ivy
RUN cd /QUIC-Ivy && python build_submodules.py
RUN pip install ms-ivy

# Add ressource file

ADD res/install_ivy.sh install_ivy.sh 
ADD res/remove_ivy.sh remove_ivy.sh 
ADD res/update_include.sh update_include.sh 
ADD res/test_client.sh test_client.sh 
ADD res/test_server.sh test_server.sh 
ADD res/test_all.sh test_all.sh 
ADD res/test.py test.py
ADD res/ivy_to_cpp.py /usr/local/lib/python2.7/dist-packages/ivy/ivy_to_cpp.py
ADD res/ivy_cpp_types.py /usr/local/lib/python2.7/dist-packages/ivy/ivy_cpp_types.py
ADD res/install_quic_implementation.sh install_quic_implementation.sh 

# Install QUIC implementation
RUN mkdir /results
ADD results/create-csv.py /results/create-csv.py
RUN mkdir /quic 
COPY --from=intermediate picotls /quic/picotls
COPY --from=intermediate picoquic /quic/picoquic
COPY --from=intermediate quiche /quic/quiche
COPY --from=intermediate quic-go /quic/quic-go
COPY res/quic-go/main.go /quic/quic-go/client/
COPY res/quic-go/server/main.go /quic/quic-go/server/
COPY --from=intermediate openssl /quic/openssl
COPY --from=intermediate ngtcp2 /quic/ngtcp2
COPY --from=intermediate nghttp3 /quic/nghttp3
COPY res/ngtcp2/generic-client-server /generic-client-server
COPY res/ngtcp2/cert /cert
COPY --from=intermediate mvfst /quic/mvfst
# TODO should be in the first part
COPY res/mvfst/tls-keys-patch.diff /quic/mvfst/ 
COPY res/mvfst/generic /quic//mvfst/quic/samples/generic
COPY res/mvfst/samples-build-patch.diff /quic/mvfst/

RUN bash install_quic_implementation.sh;exit 0

