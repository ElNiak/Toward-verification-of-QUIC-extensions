FROM ubuntu as intermediate

# https://vsupalov.com/build-docker-image-clone-private-repo-ssh-key/
# install git
RUN apt-get update
RUN apt-get install -y git

# add credentials on build TODO
ARG SSH_PRIVATE_KEY
RUN mkdir /root/.ssh/
COPY .ssh/id_rsa /root/.ssh/id_rsa

# make sure your domain is accepted
RUN touch /root/.ssh/known_hosts
RUN ssh-keyscan github.com >> /root/.ssh/known_hosts

RUN git clone --recurse-submodules git@github.com:ElNiak/QUIC-Ivy.git --branch quic_27_upgrade
RUN mkdir QUIC-Ivy/doc/examples/quic/build
RUN mkdir QUIC-Ivy/doc/examples/quic/test/temp

FROM ubuntu:18.04

# Install dependencies

RUN apt-get update
RUN apt-get install -y apt-utils

RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix gperf pkg-config libssl-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y doxygen pkg-config faketime libscope-guard-perl libtest-tcp-perl libbrotli-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libev-dev libssl-dev libhttp-parser-dev libbsd-dev snapd
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y cmake 

RUN pip install pexpect chardet 
RUN pip install gperf

# Add ressource file

ADD res/install_ivy.sh install_ivy.sh 
ADD res/remove_ivy.sh remove_ivy.sh 
ADD res/test_client.sh test_client.sh 
ADD res/test_server.sh test_server.sh 
ADD res/test_server.sh test_all.sh 
ADD res/test.py test.py
ADD res/install_quic_implementation.sh install_quic_implementation.sh 

# Install Ivy

COPY --from=intermediate QUIC-Ivy /QUIC-Ivy
RUN cd /QUIC-Ivy && python build_submodules.py
RUN pip install ms-ivy

# Install QUIC implementation
RUN mkdir /quic && cd /quic
RUN mkdir /results
RUN bash install_quic_implementation.sh;exit 0

