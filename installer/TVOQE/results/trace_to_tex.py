from scapy.all import *
import threading
import multiprocessing
from datetime import date
import pandas as pd
import os
import scandir
import pyshark




foldername = "/home/chris/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/pass/server/local/stream/"
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
					   disable_protocol="http2", 
					   decode_as={"udp.port==4443": "quic"},
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
                    if hasattr(p["quic"], "decryption_failed"):
                        print("At least one QUIC packet could not be decrypted")
                        print(p)
                        break

            f = open(outputFile, "w")
            f.write('\\begin{table}[h!]\n')
            f.write('\centering\n')
            f.write('\label{tab:my-table}\n')
            f.write('\\resizebox{\\textwidth}{!}{%\n')
            f.write('\\begin{tabular}{|cclll|}\n')
            f.write('\hline\n')
            f.write('\multicolumn{1}{|c|}{\\textbf{src}} & \multicolumn{1}{c|}{\\textbf{dst}} & \multicolumn{1}{c|}{\\textbf{time}} & \multicolumn{1}{}{} & \multicolumn{1}{c|}{\\textbf{Information}} \\\\ \hline \n')
            print(len(packets))
            print(packets)
            frames = "" #pkt.quic.frame.split(" ")[0] + " "
            last_pnum = "0"
            first = True
            for pkt in packets:
                for layer in pkt:
                    if layer.layer_name == 'quic':
                       # print(pkt.quic)
                       # print(f'Index: {pkt.number}')
                       # print(f'Timestamp: {pkt.sniff_time}')
                       # print(f'Bytes: {pkt.length}')
                       # print(f'Layers: {pkt.layers}')
                        dst_port = pkt.udp.dstport
                        src_port = pkt.udp.srcport
                        print(pkt.quic.__dict__)
                        ln = pkt.quic.packet_length
                        typ = pkt.quic.header_form
                        if hasattr(pkt.quic, "long_packet_type"):
                        	typ = pkt.quic.long_packet_type
                        dcid = pkt.quic.dcid.replace(":","")
                        pkt_t = ""
                        pkt_num = pkt.quic.packet_number
                        scid = None
                        line = ""

                        if last_pnum == pkt_num and not first:
                        	 fr  = pkt.quic.frame
                        	 frame = ""
                        	 if "TLS" in fr:
                        	 	frame = "CRYPTO"
                        	 elif "PATH_RESPONSE" in fr:
                        	 	frame = "PR"
                        	 elif "PATH_CHALLENGE" in fr:
                        	 	frame = "PC"
                        	 elif "HANDSHAKE_DONE" in fr:
                        	 	frame = "DONE"
                        	 elif "NEW_TOKEN" in fr:
                        	 	frame = "NT"
                        	 elif "MAX_STREAM" in fr:
                        	 	frame = "MS"
                        	 elif "STREAM" in fr:
                        	 	print(fr)
                        	 	stream_id = fr.split(" ")[1]
                        	 	stream_id = stream_id.split("=")[1]
                        	 	frame = "STREAM(" + stream_id + ")"
                        	 elif "NEW_CONNECTION_ID" in fr:
                        	 	frame = "NCI"
                        	 elif "RETIRE_CONNECTION_ID" in fr:
                        	 	frame = "RCI"
                        	 else:
                        	 	frame = fr
                        	 frames = frames + frame + " "

                        else:
                        	 if first:
                        	 	last_pnum = pkt.quic.packet_number
                        	 	fr  = pkt.quic.frame
                        	 	frame = ""
                        	 	if "TLS" in fr:
                        	 		frame = "CRYPTO"
                        	 	elif "PATH_RESPONSE" in fr:
                        	 		frame = "PR"
                        	 	elif "PATH_CHALLENGE" in fr:
                        	 		frame = "PC"
                        	 	elif "HANDSHAKE_DONE" in fr:
                        	 		frame = "DONE"
                        	 	elif "NEW_TOKEN" in fr:
                        	 		frame = "NT"
                        	 	elif "MAX_STREAM" in fr:
                        	 		frame = "MS"
                        	 	elif "STREAM" in fr:
                        	 		print(fr)
                        	 		stream_id = fr.split(" ")[1]
                        	 		stream_id = stream_id.split("=")[1]
                        	 		frame = "STREAM(" + stream_id + ")"
                        	 	elif "NEW_CONNECTION_ID" in fr:
                        	 		frame = "NCI"
                        	 	elif "RETIRE_CONNECTION_ID" in fr:
                        	 		frame = "RCI"
                        	 	else:
                        	 		frame = fr
                        	 	frames = frames + frame + " "
                        	 #else:
                        	 #	last_pnum = pkt.quic.packet_number
                        	 first = False
                        	 print(typ)
                        	 if typ == '0' and hasattr(pkt.quic, "long_packet_type"):
                        	 	pkt_t = "Initial"
                        	 	scid = pkt.quic.scid.replace(":","")
                        	 	line = str(ln) + ", " + pkt_t + ", DCID=" + str(dcid)+ ", SCID=" + str(scid) + ", PKT: " + str(last_pnum)  + ", " + frames
                        	 elif typ == '0':
                        	 	pkt_t = "Protected Payload (KP0)"
                        	 	line = str(ln) + ", " + pkt_t + ", DCID=" + str(dcid) + ", PKT: " + str(last_pnum)+ ", " + frames
                        	 elif typ == '2':
                        	 	pkt_t = "Handshake"
                        	 	scid = pkt.quic.scid.replace(":","")
                        	 	line = str(ln) + ", " + pkt_t + ", DCID=" + str(dcid)+ ", SCID=" + str(scid) + ", PKT: " + str(last_pnum)+ ", " + frames
	    
                        	 print(line)
                        	 print(pkt.quic.frame)
                        	 print(pkt.quic.frame_type)

                        	 if src_port == "4443":
                        	 	f.write('\\rowcolor{LightCyan} ' + str(src_port) + " & " + str(dst_port)+ " &  & \multicolumn{1}{c}{} &" )
                        	 	f.write(line)
                        	 	f.write('\\\\ \n')
                        	 else:
                        	 	f.write('\\rowcolor{lightgreen} ' + str(src_port) + " & " + str(dst_port)+ " &  & \multicolumn{1}{c}{} &" )
                        	 	f.write(line)
                        	 	f.write('\\\\ \n')
                        	 frames = ""
                        	 last_pnum = pkt_num
		                #for key in pkt.quic.__dict__:
		                #	if 'quic.padding_length' in key:
		                #		frames = frames + "PADDING" + " "
		                #	if 'quic.stream.stream_id' in key:
		                #		frames = frames + "STREAM(" + pkt.quic[key] + ") "
		                #print(pkt.info)
                        	 fr  = pkt.quic.frame
                        	 frame = ""
                        	 if "TLS" in fr:
                        	 	frame = "CRYPTO"
                        	 elif "PATH_RESPONSE" in fr:
                        	 	frame = "PR"
                        	 elif "PATH_CHALLENGE" in fr:
                        	 	frame = "PC"
                        	 elif "HANDSHAKE_DONE" in fr:
                        	 	frame = "DONE"
                        	 elif "NEW_TOKEN" in fr:
                        	 	frame = "NT"
                        	 elif "MAX_STREAM" in fr:
                        	 	frame = "MS"
                        	 elif "STREAM" in fr:
                        	 	print(fr)
                        	 	stream_id = fr.split(" ")[1]
                        	 	stream_id = stream_id.split("=")[1]

                        	 	frame = "STREAM(" + stream_id + ")"
                        	 elif "NEW_CONNECTION_ID" in fr:
                        	 	frame = "NCI"
                        	 elif "RETIRE_CONNECTION_ID" in fr:
                        	 	frame = "RCI"
                        	 else:
                        	 	frame = fr
                        	 #elif "STOP_SENDING" in frame:
                        	 #	frame = "SD"
                        	 #elif "RESET_STREAM" in frame:
                        	 #	frame = "RST"
                        	 frames = frames + frame + " "

            f.write('\hline\n')
            f.write('\end{tabular}%\n')
            f.write('}\n')
            f.write('\end{table}\n')
            f.close()
