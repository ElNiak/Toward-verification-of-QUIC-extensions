#!/bin/sh
#http://microsoft.github.io/ivy/examples/helloworld.html

file_ivy=$1                 #e.g helloworld.ivy
echo $file_ivy
file=$(echo "$file_ivy" | cut -f 1 -d '.')
echo $file
ivy_to_cpp target=repl $file_ivy
all_file="${file}.*"
ls $all_file
g++ -o $file "${file}.cpp" -lpthread
./$file

rm "${file}.cpp" #TODO
rm "${file}.h"

