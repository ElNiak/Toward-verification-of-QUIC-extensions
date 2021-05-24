from scapy.all import *
import threading
import multiprocessing
from datetime import date
import pandas as pd
import os
import scandir
import pyshark


def replace_tag(fr):
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
    return frame


def write_header_tex(writer):
    writer.write('\\begin{table}[h!]\n')
    writer.write('\centering\n')
    writer.write('\label{tab:my-table}\n')
    writer.write('\\resizebox{\\textwidth}{!}{%\n')
    writer.write('\\begin{tabular}{|cclll|}\n')
    writer.write('\hline\n')
    writer.write('\multicolumn{1}{|c|}{\\textbf{src}} & \multicolumn{1}{c|}{\\textbf{dst}} & \multicolumn{1}{c|}{\\textbf{time}} & \multicolumn{1}{}{} & \multicolumn{1}{c|}{\\textbf{Information}} \\\\ \hline \n')

def write_footer_tex(writer):
    writer.write('\hline\n')
    writer.write('\end{tabular}%\n')
    writer.write('}\n')
    writer.write('\end{table}\n')

def get_line(frames, last_pnum, pkt, pkt_len, pkt_type, dcid):
    if pkt_type == '0' and hasattr(pkt.quic, "long_packet_type"):
        pkt_t = "Initial"
        scid = pkt.quic.scid.replace(":", "")
        line = str(pkt_len) + ", " + pkt_t + ", DCID=" + str(dcid) + ", SCID=" + str(
                                    scid) + ", PKT: " + str(last_pnum) + ", " + frames
    elif pkt_type == '0':
        pkt_t = "Protected Payload (KP0)"
        line = str(pkt_len) + ", " + pkt_t + ", DCID=" + str(dcid) + \
                                    ", PKT: " + str(last_pnum) + ", " + frames
    elif pkt_type == '2':
        pkt_t = "Handshake"
        scid = pkt.quic.scid.replace(":", "")
        line = str(pkt_len) + ", " + pkt_t + ", DCID=" + str(dcid) + ", SCID=" + str(
                                    scid) + ", PKT: " + str(last_pnum) + ", " + frames
    return line

def write_line(f, dst_port, src_port, line):
    if src_port == "4443":
        f.write('\\rowcolor{LightCyan} ' + str(src_port) + " & " + str(
                                    dst_port) + " &  & \multicolumn{1}{c}{} &")
        f.write(line)
        f.write('\\\\ \n')
    else:
        f.write('\\rowcolor{lightgreen} ' + str(src_port) + " & " + str(
                                    dst_port) + " &  & \multicolumn{1}{c}{} &")
        f.write(line)
        f.write('\\\\ \n')

foldername = "/home/chris/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/pass/server/local/stream/"
subfolders = [f.path for f in scandir.scandir(foldername) if f.is_dir()]
run = 0



for fol in subfolders:
    for file in os.listdir(fol):
        print(file)
        if file.endswith(".pcapng"):
            file = str(fol) + "/" + file
            outputFile = file.replace(".pcapng", ".txt")
            override_prefs = {}

            for filessl in os.listdir(fol):
                if filessl.endswith(".log"):
                    override_prefs["ssl.keylog_file"] = str(
                        fol) + "/" + filessl
                    print(override_prefs["ssl.keylog_file"])

            cap = pyshark.FileCapture(
                file,
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

            if override_prefs["ssl.keylog_file"] is not None:
                for p in packets:
                    if hasattr(p["quic"], "decryption_failed"):
                        print("At least one QUIC packet could not be decrypted")
                        print(p)
                        break

            f = open(outputFile, "w")
            write_header_tex(f)


            frames = ""  
            last_pnum = "0"
            first = True

            for pkt in packets:
                for layer in pkt:
                    if layer.layer_name == 'quic':

                        dst_port = pkt.udp.dstport
                        src_port = pkt.udp.srcport

                        pkt_len = pkt.quic.packet_length
                        pkt_type = pkt.quic.header_form
                        if hasattr(pkt.quic, "long_packet_type"):
                            pkt_type = pkt.quic.long_packet_type
                        
                        dcid = pkt.quic.dcid.replace(":", "")
                        pkt_t = ""
                        pkt_num = pkt.quic.packet_number
                        scid = None
                        line = ""

                        if first:
                            last_pnum = pkt.quic.packet_number
                            fr = pkt.quic.frame
                            frame = replace_tag(fr)
                            frames = frames + frame + " "
                            first = False
                            # line = get_line(frames, last_pnum, pkt, pkt_len, pkt_type, dcid)
                            # write_line(f, dst_port, src_port, line)   
                        elif last_pnum == pkt_num:
                            fr = pkt.quic.frame
                            frame = replace_tag(fr)
                            frames = frames + frame + " "
                        else:
                            line = get_line(frames, last_pnum, pkt, pkt_len, pkt_type, dcid)
                            write_line(f, dst_port, src_port, line)
                            last_pnum = pkt_num
                            fr = pkt.quic.frame
                            frame = replace_tag(fr)
                            frames =  frame + " "

            write_footer_tex(f)
            f.close()
