import sys
import os
from os import listdir
from os.path import isfile, join
import shutil

#https://stackoverflow.com/questions/3346430/what-is-the-most-efficient-way-to-get-first-and-last-line-of-a-text-file/3346788
def readlastline(f):
    f.seek(-2, 2)              # Jump to the second last byte.
    while f.read(1) != b"\n":  # Until EOL is found ...
        f.seek(-2, 1)          # ... jump back, over the read byte plus one more.
    return f.read()            # Read all data from this point on.


foldername = sys.argv[1]

try:
    os.mkdir(foldername+"/pass")
    os.mkdir(foldername + "/fail")
except OSError:
    print ("Creation of the directory %s failed" % path)
else:
    print ("Successfully created the directory %s " % path)

# TODO make more clean
for file in os.listdir(foldername):
    if file.endswith(".iev"):
        fullPath = os.path.join(foldername, file)
        out = os.path.join(foldername, file.replace(".iev",".out"))
        err = os.path.join(foldername, file.replace(".iev", ".err"))
        with open(fullPath, "r") as f:
            last = readlastline(f)
            if last == "test_completed":
                newFullPathPass = os.path.join(foldername + "/pass", file)
                fullOutPass = os.path.join(foldername + "/pass", out)
                fullErrPass = os.path.join(foldername + "/pass", err)
                os.rename(fullPath, newFullPathPass)
                shutil.move(fullPath,newFullPathPass)
                os.replace(fullPath, newFullPathPass)

                os.rename(out, fullOutPass)
                shutil.move(out, fullOutPass)
                os.replace(out, fullOutPass)

                os.rename(err, fullErrPass)
                shutil.move(err, fullErrPass)
                os.replace(err, fullErrPass)
            else:
                newFullPathFail = os.path.join(foldername + "/fail", file)
                fullOutFail = os.path.join(foldername + "/fail", out)
                fullErrFail = os.path.join(foldername + "/fail", err)

                os.rename(fullPath, newFullPathFail)
                shutil.move(fullPath,newFullPathFail)
                os.replace(fullPath, newFullPathFail)

                os.rename(out, fullOutFail)
                shutil.move(out, fullOutFail)
                os.replace(out, fullOutFail)

                os.rename(err, fullErrFail)
                shutil.move(err, fullErrFail)
                os.replace(err, fullErrFail)

