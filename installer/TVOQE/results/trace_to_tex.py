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
        print(file)
        if file.endswith(".pcapng"):
            file = str(fol) + "/" +file
            outputFile = file.replace(".pcapng",".txt")
            override_prefs = {}
            for filessl in os.listdir(fol):
            	if filessl.endswith(".log"):
            		override_prefs["ssl.keylog_file"] =  str(fol) + "/" + filessl
            		print(override_prefs["ssl.keylog_file"])
            cap  =  pyshark.FileCapture(file,
         				   override_prefs=override_prefs,
				           display_filter="udp.port == 4443",
					   #disable_protocol="tcp", 
					   #decode_as={"udp.port==4443": "quic"},
					   )

            cap.set_debug()
            packets = []
            try:
                for p in cap:
                    packets.append(p)
                cap.close()
            except Exception as e:
                print(e)
            #print(packets)
            if override_prefs["ssl.keylog_file"] is not None:
                for p in packets:
                    #print(p)
                    if hasattr(p["udp"], "decryption_failed"):
                        print("At least one QUIC packet could not be decrypted")
                        print(p)
                        break

            f = open(outputFile, "a")
            f.write('\begin{table}[h!]\n')
            f.write('\centering\n')
            f.write('\label{tab:my-table}\n')
            f.write('\resizebox{\textwidth}{!}{%\n')
            f.write('\begin{tabular}{|cclll|}\n')
            f.write('\hline\n')
            f.write('\multicolumn{1}{|c|}{\textbf{src}} & multicolumn{1}{c|}{\textbf{dst}} & multicolumn{1}{c|}{\textbf{time}} & multicolumn{1}{}{} & multicolumn{1}{c|}{\textbf{Information}} \\ \hline \n')
            print(len(packets))
            print(packets)
            for pkt in packets:
                for layer in pkt:
                    if layer.layer_name == 'udp':
                        print(pkt.data)
                       # print(f'Index: {pkt.number}')
                       # print(f'Timestamp: {pkt.sniff_time}')
                       # print(f'Bytes: {pkt.length}')
                       # print(f'Layers: {pkt.layers}')
                        dst_port = pkt.udp.dstport
                        src_port = pkt.udp.srcport
                       # print(pkt)
                        if src_port == 4443:
                            f.write('\rowcolor{LightCyan} ' + str(src_port) + " & " + str(dst_port)+ " &  & \multicolumn{1}{c}{} &" )
                            #f.write(pkt.udp.summary_line)
                            f.write('\\ \n')
                        else:
                            f.write('\rowcolor{lightgreen} ' + str(src_port) + " & " + str(dst_port)+ " &  & \multicolumn{1}{c}{} &" )
                            #f.write(pkt.udp.summary_line)
                            f.write('\\ \n')
                        

            f.write('\end{tabular}%\n')
            f.write('}\n')
            f.write('\end{table}\n')
            f.close()
