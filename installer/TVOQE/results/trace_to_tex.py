from scapy.all import *
import threading
import multiprocessing
from datetime import date
import pandas as pd
import os
import scandir
import pyshark


foldername = "/home/student/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/pass/server/local/stream/"
subfolders = [f.path for f in scandir.scandir(foldername) if f.is_dir()]
run = 0
for fol in subfolders:
    for file in os.listdir(fol):
        if file.endswith(".pcap"):
            outputFile = file.replace(".pcap",".txt")
            packets =  pyshark.FileCapture(file)

            f = open(outputFile, "a")
            f.write('\begin{table}[h!]\n')
            f.write('\centering\n')
            f.write('\label{tab:my-table}\n')
            f.write('\resizebox{\textwidth}{!}{%\n')
            f.write('\begin{tabular}{|cclll|}\n')
            f.write('\hline\n')
            f.write('\multicolumn{1}{|c|}{\textbf{src}} & multicolumn{1}{c|}{\textbf{dst}} & multicolumn{1}{c|}{\textbf{time}} & multicolumn{1}{}{} & multicolumn{1}{c|}{\textbf{Information}} \\ \hline \n')

            for pkt in packets:
                print(f'Index: {pkt.number}')
                print(f'Timestamp: {pkt.sniff_time}')
                print(f'Bytes: {pkt.length}')
                print(f'Layers: {pkt.layers}')
                for layer in pkt:
                    if layer.layer_name == 'quic':
                        dst_port = pkt.quic.dstport
                        src_port = pkt.quic.srcport

                        if src_port == 4443:
                            f.write('\rowcolor{LightCyan} ' + str(src_port) + " & " + str(dst_port)+ " &  & \multicolumn{1}{c}{} &" )
                            f.write(pkt.summary_line)
                            f.write('\\ \n')
                        else:
                            f.write('\rowcolor{lightgreen} ' + str(src_port) + " & " + str(dst_port)+ " &  & \multicolumn{1}{c}{} &" )
                            f.write(pkt.summary_line)
                            f.write('\\ \n')
                        

            f.write('\end{tabular}%\n')
            f.write('}\n')
            f.write('\end{table}\n')
            f.close()
