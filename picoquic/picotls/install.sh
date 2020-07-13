#!/bin/sh

sudo apt-get install libbrotli-dev

sudo apt-get install faketime libscope-guard-perl libtest-tcp-perl

cmake .

make

make check
