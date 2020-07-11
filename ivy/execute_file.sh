#!/bin/sh
#http://microsoft.github.io/ivy/examples/helloworld.html

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Illegal number of parameters"
fi

folder_file=$1
file_ivy=$(basename $folder_file)   
folder=$(dirname $folder_file)           
echo "\nFolder: ${folder_file}"
echo "File  : ${file_ivy}"
file=$(echo "$file_ivy" | cut -f 1 -d '.')
cd $folder

echo "\n Mode:"
if [ "$#" -eq 1 ]; then
    echo "  ivy_to_cpp target=repl\n"
    ivy_to_cpp target=repl $file_ivy
fi

if [ "$#" -eq 2 ]; then
    echo "  ivy_to_cpp target=repl isolate=iso_impl\n"
    ivy_to_cpp target=repl isolate=iso_impl $file_ivy
fi

all_file="${file}.*"
echo 'Files generated:'
ls $all_file
g++ -o $file "${file}.cpp" -lpthread
./$file

rm "${file}.cpp" #TODO
rm "${file}.h"

