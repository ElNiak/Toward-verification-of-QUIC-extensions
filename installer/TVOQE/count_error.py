import sys
import os

filename = sys.argv[1]
file = open(filename, "r")

lines = {}

for line in file:
  line = line.strip("\n")
  if line in lines:
    val = lines[line]
    lines[line] = val + 1
  elif line:
    lines[line] = 1

file.close()

for item in lines:
    print("\nKey : {} , Value : {}\n".format(item,lines[item]))
