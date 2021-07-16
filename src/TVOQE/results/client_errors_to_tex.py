import threading
import multiprocessing
from datetime import date
import pandas as pd
import os
import scandir


global_test = [
    'quic_client_test_stream',
    'quic_client_test_max',
    'quic_client_test_accept_maxdata',
    'quic_client_test_ext_min_ack_delay',
]

unk_test = [
    'quic_client_test_unkown',
    'quic_client_test_tp_unkown',
]

tp_error_test = [
    'quic_client_test_double_tp_error',
    'quic_client_test_tp_error',
    'quic_client_test_tp_acticoid_error',
    'quic_client_test_no_ocid',
    'quic_client_test_tp_prefadd_error',

]

prot_error_test = [
    'quic_client_test_new_token_error',
    'quic_server_test_max_limit_error',
]

field_error_test = [
    'quic_client_test_blocked_streams_maxstream_error',
    'quic_client_test_retirecoid_error',
]


def write_header_results(writer,title):
    writer.write('\\begin{table}[h!]\n')
    writer.write('\centering\n')
    writer.write('\label{tab:client-summary-'+ title + '}\n')
    writer.write('\scriptsize\n')
    writer.write('\\resizebox{\\textwidth}{!}{%\n')
    writer.write('\\begin{tabular}{|l|cccccccc|}\n')
    writer.write('\hline\n')
    writer.write('\multicolumn{1}{|c|}{} & \multicolumn{1}{c|}{quinn} & \multicolumn{1}{c|}{mvfst} & \multicolumn{1}{c|}{picoquic} & \multicolumn{1}{c|}{quic-go} & \multicolumn{1}{c|}{aioquic} & \multicolumn{1}{c|}{quant} & \multicolumn{1}{c|}{quiche} & lsquic \\\\ \hline  \n')

def write_footer_results(writer, title):
    writer.write('\end{tabular}%\n')
    writer.write('}\n')
    writer.write('\caption{client - '+ title +'}\n')
    writer.write('\end{table}\n')

def write_summary_line(writer , testname, results, line,total_len):
    orig_testname = testname 
    testname = testname.replace("quic_client_test_", "")
    testname = testname.replace("_",'\_')

    implementation = ["quinn", "mvfst", "picoquic", "quic-go", "aioquic", "quant", "quiche", "lsquic"]

    writer.write(testname + ' & \n')

    for impl in implementation: #TODO percentage
        color = ""
        if impl+orig_testname in total_len:
            res = results[impl]/total_len[impl+orig_testname] * 100
        else:
            res = -1
        if res > 90:
            color = "009933" # Dark green
        elif res > 75 and res <= 90 :
            color = "00ff55" # green
        elif res > 60 and res <= 75 :
            color = "ffff00" # yellow
        elif res > 40 and res <= 60 :
            color = "ffbb33" # orange
        elif res > 25 and res <= 40 :
            color = "e69900" # dark orange
        elif res > 10 and res <= 25 :
            color = "ff6666" # light red
        elif res == -1 :
            color = "ffffff" # white
        else:
            color = "FE0000" # red
        res = round(res)
        if impl != "lsquic":
            writer.write('\cellcolor[HTML]{'+ color +'} '+ str(res) +'\%  & \n')
        else:
            writer.write('\cellcolor[HTML]{'+ color +'} '+ str(res) +'\%  \\\\ '+ line +'\n')


def write_header_tex(writer,implementation, title):
    writer.write('\\begin{table}[h!]\n')
    writer.write('\centering\n')
    writer.write('\label{tab:client-'+ implementation + '}\n')
    writer.write('\scriptsize\n')
    writer.write('\caption{\lstinline{' + implementation +'} client errors - '+ title +'}')
    writer.write('\\resizebox{\\textwidth}{!}{%\n')
    writer.write('\\begin{tabular}{llr}\n')
    #writer.write('\hline\n')
    writer.write('\multicolumn{1}{c}{\\textbf{Test}} & \multicolumn{1}{c}{\\textbf{Error code}} \multicolumn{1}{c}{\\textbf{\#}} \\\\ \n')

def write_footer_tex(writer):
    writer.write('\end{tabular}%\n')
    writer.write('}\n')
    writer.write('\end{table}\n')

def write_result(writer, nb,total_len):
    writer.write('  &       & \\textbf{' + str(nb) + '/' +str(total_len) +'}      \\\\ \n')
    writer.write('   &       &   \\\\ \n')

def write_result_no_error(writer, testname,total_len):
    testname = testname.replace("_",'\_')
    writer.write(testname +' &       & \\textbf{0.0/' +str(total_len) +'}      \\\\ \n')
    writer.write('   &       &   \\\\ \n')

def write_line(writer, error, nb):
    error = error.replace("_",'\_')
    error = error.replace("}",'\}')
    error = error.replace("{",'\{')
    error = error.replace("&",'\&')
    error = error.replace("~","$\sim$")
    error = error.replace(">","\\textgreater")
    writer.write('                           & ' + error +    ' & ' +  str(nb)      + '  \\\\ \n')

def write_test_line(writer, testname, error, nb):
    error = error.replace("_",'\_')
    error = error.replace("}",'\}')
    error = error.replace("{",'\{')
    error = error.replace("~",'$\sim$')
    error = error.replace("&",'\&')
    error = error.replace(">","\\textgreater")
    testname = testname.replace("_",'\_')
    writer.write(  testname +' & ' + error +' & '+ str(nb)   + '  \\\\ \n')

def filter_test(train_df):
    for i, row in train_df.iterrows():
        if isinstance(row["TestName"], str):
            train_df.loc[i, "TestName"] = row["TestName"].replace("0","")
        else:
            index = row["OutputFile"].find("q")
            train_df.loc[i, "TestName"] = row["OutputFile"][index:].replace("0","").replace(".iev","")
    train_df = train_df[train_df.TestName != 'quic_client_test_tp_limit_acticoid_error']
    train_df = train_df[train_df.TestName != 'quic_client_test_stop_sending_error']
    train_df = train_df[train_df.TestName != 'quic_client_test_crypto_limit_error']
    train_df = train_df[train_df.TestName != 'quic_client_test_newcoid_zero_error']
    return train_df

def extract_implementation(train_df):
    client_name = train_df.Implementation.unique()
    print(client_name)
    pd.options.display.max_colwidth = 200
    #print(train_df[train_df["Implementation"].isna()]["OutputFile"])
    clients = []
    for s in client_name:
        if "cargo run --manifest-path=tools/apps/Cargo.toml" in s:
            clients.append("quiche")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quiche")
        elif "cargo run --example client" in s or "cargo run  -vv --example client https://localhost:4443/index.html --keylog" in s or "cargo run -vv --example client https://local" in s:
            clients.append("quinn")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quinn")
        elif "./http_client" in s:
            clients.append("lsquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "lsquic")
        elif "./picoquicdemo" in s:
            clients.append("picoquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "picoquic")
        elif "./client -c" in s or "/quant/Debug/bin/client" in s or "./client -l /results/temp/quant_key.log -c false -r 20 -u  -t 3600 -v 5 -e 0xff00001" in s:
            clients.append("quant")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quant")
        elif "./client -X" in s or "./client -p 4443 127.0.0.1" in s or "./client -G 5000000 -X /logs.txt" in s:
            clients.append("quic-go")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quic-go")
        elif "python3" in s:
            clients.append("aioquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "aioquic")
        elif "echo" in s:
            clients.append("mvfst")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "mvfst")
    return clients

def extract_file_name(path):
    splited = path.split("/")
    filename = splited[-2]+ "/" + splited[-1] 
    return filename

def extract_error(foldername, train_df):
    #train_df = train_df.set_index("Run")
    for i, row in train_df.iterrows():
        if row["isPass"] == 0.0:
            resultFile =  foldername +"temp/" + extract_file_name(row["OutputFile"])     

            f = open(resultFile, "r")
            content = f.read()
         
            start_index = content.find('TEST_DCIL')
            end_index = content.find('\n',start_index)
            c = content[start_index:end_index+1]
            c = c.replace('TEST_DCIL',"")
            c = c.replace(' ',"")
            c = c.replace('\n',"")
            #print(c)
            if c != '':
                 c = int(c)
                 if c > 16:
                      #print(i)
                      train_df.drop(i, axis=0, inplace=True)
                      continue

            if row["Implementation"] == "quiche":
                #In fact we only do unidirectionnal with quiche
                if "require conn_total_data(the_cid) > 0;" in content and "stream.handle" in content: 
                    train_df.loc[i, "ErrorIEV"] = "No error"
                    train_df.loc[i, "isPass"] = 1.0
                    continue
            elif row["Implementation"] == "aioquic" :
                #In fact we only do unidirectionnal with quiche
                if "quic_client_test_stream" in resultFile and content.count("stream.handle") > 5 and  content.count("undecryptable_packet_event") < 1 : 
                    train_df.loc[i, "ErrorIEV"] = "No error"
                    train_df.loc[i, "isPass"] = 1.0
                    continue
            if "frame.connection_close:" in content and not "quic_client_test_connection_close" in resultFile:
                # Extract CONNECTION_CLOSE frame
                start_index = content.rfind("frame.connection_close:")
                end_index = content.find(",",start_index)
                train_df.loc[i, "ErrorIEV"] = content[start_index:end_index+1].replace(",","") + "}"
                #print("conne erro")
            elif "frame.application_close:" in content and not "quic_client_test_connection_close" in resultFile:
                # Extract CONNECTION_CLOSE frame
                start_index = content.rfind("frame.application_close:")
                end_index = content.find(",",start_index)
                train_df.loc[i, "ErrorIEV"] = content[start_index:end_index+1].replace(",","") + "}"
                #print("conne erro")
            elif "server_return_code(1)+ivy_return_code(1)" in content:
                 print("coooool")
                 train_df = train_df.drop(i)
            elif "ivy_return_code(139)" in content:
                train_df.loc[i, "ErrorIEV"] = "ivy_return_code(139)"
            elif "ivy_return_code(134)" in content:
                train_df.loc[i, "ErrorIEV"] = "ivy_return_code(134)"
            elif "server_return_code(" in content:
                start_index = content.find('server_return_code(')
                end_index = content.find(')',start_index)
                c = content[start_index:end_index+1]
                train_df.loc[i, "ErrorIEV"] = c
            elif "client_return_code(" in content:
                start_index = content.find('client_return_code(')
                end_index = content.find(')',start_index)
                c = content[start_index:end_index-1]
                train_df.loc[i, "ErrorIEV"] = c
            elif "server_return_code(254)+ivy_return_code(1)" in content:
                 train_df.loc[i, "ErrorIEV"] = "No decryption keys + server_return_code(254)"
            elif "test_completed" in content:
                 train_df.loc[i, "ErrorIEV"] = 'No Error'
                 train_df.loc[i, "isPass"] = 1.0
            elif "quic_packet.ivy: line 421" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require ~_generating & ~queued_non_ack(scid) -> ack_credit(scid) > 0;  # [5]'
            elif "quic_packet.ivy: line 415" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require  conn_seen(dcid) -> hi_non_probing_endpoint(dcid,dst);  # [10]'
            elif "quic_frame.ivy: line 597" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 1494" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require ~path_challenge_pending(dcid,f.data);'
            elif "quic_frame.ivy: line 1468" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 1358" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 1358" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 723" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require f.fin <-> (stream_app_data_finished(dcid,f.id) & offset+f.length = stream_app_data_end(dcid,f.id));'

            elif "quic_client_test_new_token_error.ivy: line 390" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_client_test_no_odci.ivy: line 495" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_transport_parameter_error;'
            elif "quic_client_test_new_token_error.ivy: line 517" in content or "quic_client_test_retirecoid_error.ivy: line 276" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_client_test_max_limit_error.ivy: line 527" in content or "quic_client_test_limit_max_error.ivy: line 501" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_client_test_max_error.ivy: line 517" in content or "quic_client_test_limit_max_error.ivy: line 504" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_client_test_newcoid_rtp_error.ivy: line" in content or "quic_client_test_tp_error.ivy: line 504" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_client_test_newcoid_length_error.ivy: line 527" in content or "quic_client_test_tp_prefadd_error.ivy: line 518" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_client_test_unkown.ivy: line 571" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_client_test_newconnectionid_error.ivy: line 504" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
            elif "quic_client_test_retirecoid_error.ivy: line 507" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_protocol_violation;'
            elif "quic_client_test_double_tp_error.ivy: line 502" in content or "quic_client_test_tp_acticoid_error.ivy: line 515" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_transport_parameter_error;'
            elif "quic_client_test_tp_error.ivy: line 502" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_transport_parameter_error;'
            elif "quic_client_test_blocked_streams_maxstream_error.ivy: line 283" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error | is_stream_limit_error;'
            elif "quic_frame.ivy: line 1600" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require f.seq_num > last_ack_freq_seq(scid);'
            elif "quic_frame.ivy: line 1539" in content:
                 train_df.loc[i, "ErrorIEV"] =  "require is_frame_encoding_error;" #'require path_challenge_pending(scid,f.data);'
                 print(row)
            elif "quic_packet.ivy: line 377" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require pkt.long & connected(dcid) -> connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 738" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require stream_id_allowed(dcid,f.id);  # [6]'
            elif "quic_frame.ivy: line 707" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require ~conn_closed(scid);  # [8]'
            elif "quic_frame.ivy: line 798" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require ~conn_closed(scid);  # [1]' 
            elif "quic_client_test_ext_min_ack_delay.ivy: line 524" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
            elif "quic_client_test_tp_unkown.ivy: line 515" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'

            elif "quic_client_test.ivy: line 310" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
            elif "quic_client_test_unkown.ivy: line 513" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'

            elif "quic_client_test_unkown_tp.ivy: line 568" in content:
                 if row["Implementation"] == "quiche":
                    #In fact we only do unidirectionnal with quiche
                    if "stream.handle" in content: 
                       train_df.loc[i, "ErrorIEV"] = "No error"
                       train_df.loc[i, "isPass"] = 1.0
                    else:
                       train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
                 else:
                    train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
            elif "assumption_failed" in content:
                start_index = content.find('assumption_failed')
                end_index = content.find(')',start_index)
                c = content[start_index:end_index+1]
                #c = c.replace('assumption_failed(""',"")
                #print(start_index)
                #print(end_index)
                #print(content[start_index:end_index+10])
                train_df.loc[i, "ErrorIEV"] = c
            elif "ivy_return_code(139)" in content:
                train_df.loc[i, "ErrorIEV"] = "ivy_return_code(139)"
            elif "ivy_return_code(134)" in content:
                train_df.loc[i, "ErrorIEV"] = "ivy_return_code(134)"
            elif "server_return_code(" in content:
                start_index = content.find('server_return_code(')
                end_index = content.find(')',start_index)
                c = content[start_index:end_index+1]
                train_df.loc[i, "ErrorIEV"] = c
            elif "client_return_code(" in content:
                start_index = content.find('client_return_code(')
                end_index = content.find(')',start_index)
                c = content[start_index:end_index-1]
                train_df.loc[i, "ErrorIEV"] = c
            elif "ivy_return_code(1)+value(" in content:
                train_df.loc[i, "ErrorIEV"] = "Run out of cid"
            elif "timeout+> tls_recv_event({" in content:
                train_df.loc[i, "ErrorIEV"] = "Handshake not completed"
            elif "timeout+> client_send_event({" in content:
                train_df.loc[i, "ErrorIEV"] = "Handshake not completed"
            elif "timeout+< undecryptable_packet_event" in content:
                train_df.loc[i, "ErrorIEV"] = "No decryption keys"
            elif "timeout+> tls_recv_event({" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "Handshake not completed"
            elif "timeout+> client_send_event({" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "Handshake not completed"
            elif "timeout+< undecryptable_packet_event" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "No decryption keys"
            elif "timeout+sending id:" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            elif "timeout+sending id:" in content:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            elif "binding id: server addr:" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            elif "timeout+< show_pstats" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            elif "timeout+< show_pstats" in content:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            elif content.count("undecryptable_packet_event") > content.count("recv_packet"):
                train_df.loc[i, "ErrorIEV"] = "No decryption keys"
            elif content.count("undecryptable_packet_event") > content.count("recv_packet"):
                train_df.loc[i, "ErrorIEV"] = "No decryption keys"
            elif "timeout+> frame.ack.handle" in content:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            elif "timeout+> frame.ack.handle" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            #elif content.count("tls_recv_event") > 15:
                # Approximation but can induce false positive
            #    train_df.loc[i, "ErrorIEV"] = "Handshake not completed"
        else:
            train_df.loc[i, "ErrorIEV"] = "No Error"
    #for i, row in train_df.iterrows():
    #    if row["isPass"] == 0.0:
    #        train_df.loc[i, "ErrorIEV"]  = train_df.loc[i, "ErrorIEV"].replace("\n","").replace("    ","").replace("\"","") #.replace('}',"\}").replace('{',"\{")

def multiple_output(train_df):
    for i, row in train_df.iterrows():

        if row['Implementation'] == 'quiche' and "server_return_code(254)" in row["ErrorIEV"]:
            train_df.loc[i, "ErrorIEV"] = "Timeout"

        if row['Implementation'] == 'picoquic':
            resultFile =  foldername +"temp/" + extract_file_name(row["OutputFile"]).replace("iev","out")     
            f = open(resultFile, "r")
            content = f.read()
            if "Connection end with local error 0x434" in content:
                start_index = content.rfind("Connection end with local error 0x434")
                end_index = content.find(".",start_index)
                train_df.loc[i, "ErrorIEV"] = content[start_index:end_index+1].replace(".","")
                train_df.loc[i, "isPass"] = 0.0 

        if row['TestName'] == 'quic_client_test_token_error':
            if row["ErrorIEV"] == "Handshake not completed" or row["ErrorIEV"] == "Timeout":
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"

        if row['TestName'] == 'quic_client_test_new_token_error':
            if row["ErrorIEV"] == "frame.connection_close:{err_code:0x7}":
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
                print(row)
        if row['TestName'] == 'quic_client_test_blocked_streams_maxstream_error':
            if row["ErrorIEV"] == "frame.connection_close:{err_code:0x7}" or row["ErrorIEV"] == "frame.connection_close:{err_code:0x4}":
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
        if row['TestName'] == 'quic_client_test_retirecoid_error':
            if row["ErrorIEV"] == "frame.connection_close:{err_code:0xa}":
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"

        elif row['TestName'] == 'quic_client_test_double_tp_error':
            if row["ErrorIEV"] == "Handshake not completed" or row["ErrorIEV"] == "Timeout" or "frame.connection_close:{err_code:0x8}" in row["ErrorIEV"]:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
        elif row['TestName'] == 'quic_client_test_tp_error':
            if "frame.connection_close:{err_code:0x8}" in row["ErrorIEV"]:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
        elif row['TestName'] == 'quic_client_test_new_token_error':
            if "frame.connection_close:{err_code:0x7}" in row["ErrorIEV"]:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
                print(row)
        elif row['TestName'] == 'quic_client_test_unkown':
            if "frame.connection_close:{err_code:0x7}" in row["ErrorIEV"]:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
        elif row['TestName'] == 'quic_client_test_no_ocid':
            resultFile =  foldername +"temp/" + extract_file_name(row["OutputFile"]).replace("iev","out")     
            f = open(resultFile, "r")
            content = f.read()
            if "frame.connection_close:{err_code:0x8}" in row["ErrorIEV"] or "frame.connection_close:{err_code:0x8}" in content:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"

        elif row['TestName'] in global_test:
            print(row["ErrorIEV"])
            if "frame.connection_close:{err_code:0x0" in row["ErrorIEV"] or "frame.application_close:{err_code:0x0" in row["ErrorIEV"] or "frame.connection_close:{err_code:0" in row["ErrorIEV"] or "frame.application_close:{err_code:0" in row["ErrorIEV"]:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"

def pprint(str):
    print("="*50)
    print("==== " + str)
    print("="*50)

def get_errors(train_df, tests, f, total_len):
    clients = train_df.Implementation.unique() 
    tests = train_df.TestName.unique() 
    title = "Full"
    for i in clients:
        print(i)
        subdf = train_df.loc[train_df['Implementation'] == i]
        write_header_tex(f,i, title)
        for t in tests:
            print(t)
            errors = {}
            subsubdf = subdf.loc[subdf['TestName'] == t]
            cnt = 0
            for _ , row in subsubdf.iterrows():
                err = row['ErrorIEV']
                cnt += 1
                if err in errors:
                    errors[err] = errors[err] + 1
                else :
                    errors[err] = 1
            print(errors)
            first = True
            if cnt > 0:
                for error in errors:
                    if not "No Error" in error :
                        if first:
                            write_test_line(f,t,error, errors[error])
                            first = False
                        else:
                            write_line(f,error, errors[error])
                print(i)
                if (cnt - total[i+t]) > 0:
                    write_result(f, cnt - total[i+t],total_len[i+t])
                else:
                    write_result_no_error(f, t,total_len[i+t])
        write_footer_tex(f)

    last = tests[-1]
    write_header_results(f,title)
    for t in tests:
        subdf = train_df.loc[train_df['TestName'] == t]
        if len(subdf) == 0:
            summary = {
		    "quinn":-1,
		    "mvfst":-1,
		    "picoquic":-1,
		    "quic-go":-1, 
		    "aioquic":-1, 
		    "quant":-1, 
		    "quiche":-1,
            	    "lsquic":-1
		}
            if t != last:
                write_summary_line(f, t, summary, "\cline{1-1}",total_len)
            else:
                write_summary_line(f, t, summary, "\hline",total_len)
            continue   
        summary = {
            "quinn":0,
            "mvfst":0,
            "picoquic":0,
            "quic-go":0, 
            "aioquic":0, 
            "quant":0, 
            "quiche":0,
            "lsquic":0
        }
        for i in clients:
            subsubdf = subdf.loc[subdf['Implementation'] == i]
            errors = {}
            cnt = 0
            for _ , row in subsubdf.iterrows():
                err = row['ErrorIEV']
                if "No Error" in err:
                    summary[i] = summary[i] + 1
        if t != last:
            write_summary_line(f, t, summary, "\cline{1-1}",total_len)
        else:
            write_summary_line(f, t, summary, "\hline",total_len)
    write_footer_results(f,title)

def get_errors_split(train_df, tests,title, total, f, total_len):
    last = tests[-1]
    write_header_results(f,title)
    for t in tests:
        subdf = train_df.loc[train_df['TestName'] == t]
        if len(subdf) == 0:
            summary = {
		    "quinn":-1,
		    "mvfst":-1,
		    "picoquic":-1,
		    "quic-go":-1, 
		    "aioquic":-1, 
		    "quant":-1, 
		    "quiche":-1,
            	    "lsquic":-1
		}
            if t != last:
                write_summary_line(f, t, summary, "\cline{1-1}",total_len)
            else:
                write_summary_line(f, t, summary, "\hline",total_len)
            continue   
        summary = {
            "quinn":0,
            "mvfst":0,
            "picoquic":0,
            "quic-go":0, 
            "aioquic":0, 
            "quant":0, 
            "quiche":0,
            "lsquic":0
        }
        for i in clients:
            subsubdf = subdf.loc[subdf['Implementation'] == i]
            errors = {}
            cnt = 0
            for _ , row in subsubdf.iterrows():
                err = row['ErrorIEV']
                if "No Error" in err:
                    summary[i] = summary[i] + 1
        if t != last:
            write_summary_line(f, t, summary, "\cline{1-1}",total_len)
        else:
            write_summary_line(f, t, summary, "\hline",total_len)
    write_footer_results(f,title)




foldername = "/home/chris/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/errors/server/local/server-stream-limit/" 
#foldername = "/home/chris/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/"
csv_name = "May-30-2021.csv"

train_df = pd.read_csv(foldername + csv_name, index_col=0)
print(train_df.head())

# Filter test not working
pprint("Filter test not working")
train_df = filter_test(train_df)
train_df = train_df[train_df["Implementation"].notna()]
train_df = train_df[train_df["Mode"] == "client"]
# Replace with correct implementation name
pprint("Correct implementation name")


clients = extract_implementation(train_df)


print(clients)
print(train_df.head()) 

# Find the most appropriate error name
pprint("Find the most appropriate error name")
pprint(str(train_df.shape[0]))
extract_error(foldername, train_df)
pprint(str(train_df.shape[0]))
print(train_df.head())

# Preprocess multiple valid output possible
pprint("Preprocess multiple valid output possible")
multiple_output(train_df)

print(train_df.head())

# Get total per implementation & test
pprint("Get total per implementation & test")
tests = train_df.TestName.unique()
clients = train_df.Implementation.unique() 
print(tests)
print(clients)
total = {}
total_len = {}
for t in tests: 
    subdf = train_df.loc[train_df['TestName'] == t]
    for s in clients: 
        ssubdf = subdf.loc[subdf['Implementation'] == s]
        total[s+t] = ssubdf["isPass"].sum()
        total_len[s+t] = len(ssubdf.index)
        print(t)
        print(s)
        print(len(ssubdf.index))
        print(total[s+t])
        print()

outputFile = csv_name.replace("csv","txt")
f = open(outputFile, "w")
# Get total per implementation & test
pprint("Get errors count")

get_errors(train_df, total, f, total_len)

get_errors_split(train_df, global_test, "Global test", total, f,total_len)
get_errors_split(train_df, unk_test, "Unknown situation", total, f,total_len)
get_errors_split(train_df, tp_error_test, "Transport parameter errors tests", total, f,total_len)
get_errors_split(train_df, field_error_test, "Invalid fields frames errors tests", total, f,total_len)
get_errors_split(train_df, prot_error_test, "Protocol violation errors tests", total, f,total_len)


f.close()



            # Temporary
'''
            elif "quic_packet.ivy: line 421" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require ~_generating & ~queued_non_ack(scid) -> ack_credit(scid) > 0;  # [5]'
            elif "quic_packet.ivy: line 415" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require  conn_seen(dcid) -> hi_non_probing_endpoint(dcid,dst);  # [10]'
            elif "quic_frame.ivy: line 597" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 1468" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 1358" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "quic_frame.ivy: line 1358" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require connected(dcid) & connected_to(dcid) = scid;'
            elif "test_completed" in content:
                 train_df.loc[i, "ErrorIEV"] = 'No Error'
                 train_df.loc[i, "isPass"] = 1.0
            elif "quic_client_test_new_token_error.ivy: line 281" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_protocol_violation;'
            elif "quic_client_test_max_limit_error.ivy: line 527" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_client_test_max_error.ivy: line 517" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_client_test_newcoid_rtp_error.ivy: line" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_client_test_newcoid_length_error.ivy: line 527" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_client_test_newconnectionid_error.ivy: line 504" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
            elif "quic_client_test_retirecoid_error.ivy: line 507" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_protocol_violation;'
            elif "quic_client_test_unkown_tp.ivy: line 568" in content:
                 if row["Implementation"] == "quiche":
                 #In fact we only do unidirectionnal with quiche
                    if "stream.handle" in content: 
                       train_df.loc[i, "ErrorIEV"] = "No error"
                       train_df.loc[i, "isPass"] = 1.0
                    else:
                       train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
                 else:
                    train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
'''


