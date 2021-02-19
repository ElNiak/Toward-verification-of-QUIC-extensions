FROM ubuntu:18.04 as intermediate

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

RUN git clone --recurse-submodules git@github.com:ElNiak/QUIC-Ivy.git --branch quic_transport_error_code
RUN mkdir QUIC-Ivy/doc/examples/quic/build
RUN mkdir QUIC-Ivy/doc/examples/quic/test/temp

RUN git clone https://github.com/h2o/picotls.git 
WORKDIR /picotls
RUN git checkout 2464adadf28c1b924416831d24ca62380936a209 
RUN git submodule init
RUN git submodule update
WORKDIR /
RUN git clone https://github.com/private-octopus/picoquic.git 
WORKDIR /picoquic 
RUN git checkout 639c9e685d37e74d357d3dd8599b9dbff90934af

FROM ubuntu:18.04

# Install dependencies

RUN apt-get update
RUN apt-get install -y apt-utils

RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix gperf pkg-config libssl-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y doxygen pkg-config faketime libscope-guard-perl libtest-tcp-perl libbrotli-dev
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libev-dev libssl-dev libhttp-parser-dev libbsd-dev snapd
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y cmake wireshark tshark

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
ADD res/ivy_to_cpp.py ivy_to_cpp.py
ADD res/ivy_cpp_types.py ivy_cpp_types.py
ADD res/install_quic_implementation.sh install_quic_implementation.sh 

# Install QUIC implementation
RUN mkdir /results
ADD results/create-csv.py /results/create-csv.py
RUN mkdir /quic 
COPY --from=intermediate picotls /quic/picotls
COPY --from=intermediate picoquic /quic/picoquic
RUN bash install_quic_implementation.sh;exit 0

