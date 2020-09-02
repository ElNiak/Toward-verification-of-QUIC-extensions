import sys
import os
from os import listdir
from os.path import isfile, join
import shutil

#https://stackoverflow.com/questions/3346430/what-is-the-most-efficient-way-to-get-first-and-last-line-of-a-text-file/3346788
def readlastline(f):
    for line in f:
    	pass
    return line



foldername =  sys.argv[1] #"/home/chris/Toward-verification-of-QUIC-extensions/result/"+

try:
    os.mkdir(foldername+"/pass")
    os.mkdir(foldername + "/fail")
except OSError:
    print ("Creation of the directory %s failed" % foldername)
else:
    print ("Successfully created the directory %s " % foldername)

# TODO make more clean
for file in os.listdir(foldername):
    if file.endswith(".iev"):
        fullPath = os.path.join(foldername, file)
        out = file.replace(".iev",".out")
	outPath = os.path.join(foldername, out)
        err = file.replace(".iev", ".err")
	errPath = os.path.join(foldername, err)
        with open(fullPath, "r") as f:
            last = readlastline(f)
            if last in "test_completed\n":
                newFullPathPass = os.path.join(foldername + "/pass", file)
                fullOutPass = os.path.join(foldername + "/pass", out)
                fullErrPass = os.path.join(foldername + "/pass", err)

                if(os.path.exists(fullPath)): 
			shutil.move(fullPath,newFullPathPass)
                if(os.path.exists(outPath)): 
			shutil.move(outPath, fullOutPass)
                if(os.path.exists(errPath)): 
			shutil.move(errPath, fullErrPass)

            else:
                newFullPathFail = os.path.join(foldername + "/fail", file)
                fullOutFail = os.path.join(foldername + "/fail", out)
                fullErrFail = os.path.join(foldername + "/fail", err)

                if(os.path.exists(fullPath)): 
			 shutil.move(fullPath,newFullPathFail)
                if(os.path.exists(outPath)): 
			 shutil.move(outPath, fullOutFail)
                if(os.path.exists(errPath)): 
			 shutil.move(errPath, fullErrFail)


