FROM ubuntu as intermediate

# https://vsupalov.com/build-docker-image-clone-private-repo-ssh-key/
# install git
RUN apt-get update
RUN apt-get install -y git

# add credentials on build
ARG SSH_PRIVATE_KEY
RUN mkdir /root/.ssh/
RUN echo "${SSH_PRIVATE_KEY}" > /root/.ssh/id_rsa

# make sure your domain is accepted
RUN touch /root/.ssh/known_hosts
RUN ssh-keyscan github.com >> /root/.ssh/known_hosts

RUN git clone git@github.com:ElNiak/QUIC-Ivy.git
RUN cd QUIC-Ivy/
RUN git checkout quic_27_upgrade #Jan 28 2020
RUN mkdir $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/build
RUN mkdir $HOME/TVOQE_UPGRADE_27/QUIC-Ivy/doc/examples/quic/test/temp

FROM ubuntu:18.04

RUN useradd -m user
USER user
ENV HOME /home/user

WORKDIR /home/user

# Install dependencies

RUN apt-get update
RUN apt-get install -y apt-utils

RUN apt-get install -y python python-pip g++ cmake python-ply python-pygraphviz git python-tk tix gperf pkg-config libssl-dev
RUN apt-get install -y doxygen pkg-config faketime libscope-guard-perl libtest-tcp-perl libbrotli-dev
RUN apt-get install -y libev-dev libssl-dev libhttp-parser-dev libbsd-dev 
RUN snap install cmake --classic

RUN pip install --yes pexpect chardet 
RUN pip install --yes gperf

# Add ressource file

ADD res/install_ivy.sh install_ivy.sh 
ADD res/remove_ivy.sh remove_ivy.sh 
ADD res/test_client.sh test_client.sh 
ADD res/test_server.sh test_server.sh 
ADD res/test_server.sh test_all.sh 
ADD res/install_quic_implementation.sh install_quic_implementation.sh 

# Install Ivy

COPY --from=intermediate QUIC-Ivy /home/user/QUIC-Ivy
RUN cd /home/user/QUIC-Ivy && python build_submodules.py
RUN pip install --yes ms-ivy

# Install QUIC implementation
RUN mkdir /home/user/quic && cd /home/user/quic
RUN bash install_quic_implementation.sh

