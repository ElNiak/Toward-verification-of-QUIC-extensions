#Clone quic-go project
cd $HOME/TVOQE_UPGRADE_27/quic
#Install go
wget https://dl.google.com/go/go1.14.linux-amd64.tar.gz  &> /dev/null
tar xfz go1.14.linux-amd64.tar.gz &> /dev/null
rm go1.14.linux-amd64.tar.gz
#Install project
git clone https://github.com/lucas-clemente/quic-go
cd $HOME/TVOQE_UPGRADE_27/quic/quic-go
git checkout v0.18.1
export PATH="/go/bin:${PATH}"
mkdir client server
go get ./...
go build -o $HOME/TVOQE_UPGRADE_27/quic/quic-go/client/client $HOME/TVOQE_UPGRADE_27/quic/go_client/main.go
go build -o $HOME/TVOQE_UPGRADE_27/quic/quic-go/server/server $HOME/TVOQE_UPGRADE_27/quic/go_server/main.go

