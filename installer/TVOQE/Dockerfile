FROM ubuntu:18.04 as intermediate

# https://vsupalov.com/build-docker-image-clone-private-repo-ssh-key/
# install git
RUN apt-get update && apt-get -y install alien
RUN apt-get install --fix-missing -y git wget tar 

# add credentials on build TODO
ARG SSH_PRIVATE_KEY
RUN mkdir /root/.ssh/
COPY .ssh/id_rsa /root/.ssh/id_rsa
RUN chmod 400 /root/.ssh/id_rsa

# make sure your domain is accepted
RUN touch /root/.ssh/known_hosts
RUN ssh-keyscan github.com >> /root/.ssh/known_hosts

ADD src/mvfst/tls-keys-patch.diff tls-keys-patch.diff
ADD src/download_quic_implementation.sh download_quic_implementation.sh 

RUN bash download_quic_implementation.sh

FROM ubuntu:18.04

# Install dependencies

RUN apt-get update  && apt-get -y install alien
RUN apt-get install -y apt-utils

RUN DEBIAN_FRONTEND="noninteractive" apt-get --fix-missing  -y install tzdata curl wget tar 
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix gperf pkg-config libssl-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y doxygen pkg-config faketime libscope-guard-perl libtest-tcp-perl libbrotli-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y libev-dev libhttp-parser-dev libbsd-dev snapd
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y cmake wireshark tshark
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y binutils autoconf automake autotools-dev libtool pkg-config libev-dev libjemalloc-dev ca-certificates mime-support
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y libboost-all-dev libevent-dev libdouble-conversion-dev libgoogle-glog-dev libgflags-dev libiberty-dev liblz4-dev liblzma-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y libsnappy-dev zlib1g-dev binutils-dev libjemalloc-dev libsodium-dev sudo
RUN DEBIAN_FRONTEND="noninteractive" apt-get install  --fix-missing  -y git python3 python3-dev python3-pip build-essential libffi-dev python-dev cargo
RUN pip install pexpect chardet 
RUN pip install gperf pandas scandir
RUN pip3 install setuptools-rust

RUN apt remove --fix-missing  -y  cmake
RUN snap install cmake --classic -y

# Install Ivy

COPY --from=intermediate QUIC-Ivy /QUIC-Ivy
RUN cd /QUIC-Ivy && python build_submodules.py
RUN pip install ms-ivy

# Add ressource file

ADD src/install_ivy.sh install_ivy.sh 
ADD src/remove_ivy.sh remove_ivy.sh 
ADD src/update_include.sh update_include.sh 
ADD src/test_client.sh test_client.sh 
ADD src/test_server.sh test_server.sh 
ADD src/test_all.sh test_all.sh 
ADD src/test.py test.py
ADD src/ivy_to_cpp.py /usr/local/lib/python2.7/dist-packages/ivy/ivy_to_cpp.py
ADD src/ivy_cpp_types.py /usr/local/lib/python2.7/dist-packages/ivy/ivy_cpp_types.py
ADD src/install_quic_implementation.sh install_quic_implementation.sh 

# Install QUIC implementation
RUN mkdir /results
ADD results/create-csv.py /results/create-csv.py
RUN mkdir /quic /quic/certs
COPY --from=intermediate picotls /quic/picotls
COPY --from=intermediate picoquic /quic/picoquic
COPY --from=intermediate quant /quic/quant
COPY --from=intermediate quic-go /quic/quic-go
COPY src/quic-go/main.go /client/main.go
COPY src/quic-go/server/main.go /server/main.go
COPY src/quic-go/priv.key /certs/priv.key
COPY src/quic-go/cert.pem /certs/cert.pem
COPY --from=intermediate aioquic /quic/aioquic
COPY --from=intermediate mvfst /quic/mvfst
# TODO should be in the first part
COPY src/mvfst/generic /quic/mvfst/quic/samples/generic
COPY src/mvfst/samples-build-patch.diff /quic/mvfst/

RUN bash install_quic_implementation.sh;exit 0

