#!/bin/sh
#http://microsoft.github.io/ivy/examples/helloworld.html

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Illegal number of parameters"
fi

file_ivy=$1                 #e.g helloworld.ivy
echo $file_ivy
file=$(echo "$file_ivy" | cut -f 1 -d '.')
echo $file

if [ "$#" -eq 1 ]; then
    echo "ivy_to_cpp target=repl"
    ivy_to_cpp target=repl $file_ivy
fi

if [ "$#" -eq 2 ]; then
    echo "ivy_to_cpp target=repl isolate=iso_impl"
    ivy_to_cpp target=repl isolate=iso_impl $file_ivy
fi

all_file="${file}.*"
ls $all_file
g++ -o $file "${file}.cpp" -lpthread
./$file

rm "${file}.cpp" #TODO
rm "${file}.h"

