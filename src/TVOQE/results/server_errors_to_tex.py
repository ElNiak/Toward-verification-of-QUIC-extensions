import threading
import multiprocessing
from datetime import date
import pandas as pd
import os
import scandir


global_test = [
    'quic_server_test_stream',
    'quic_server_test_max',
    'quic_server_test_accept_maxdata',
    'quic_server_test_reset_stream',
    'quic_server_test_connection_close',
    'quic_server_test_stop_sending',
    'quic_server_test_ext_min_ack_delay',
]

unk_test = [
    'quic_server_test_unknown',
    'quic_server_test_unknown_tp',
]

tp_error_test = [
    'quic_server_test_double_tp_error',
    'quic_server_test_tp_error',
    'quic_server_test_tp_acticoid_error',
    'quic_server_test_no_icid',
]

prot_error_test = [
    'quic_server_test_token_error',
    'quic_server_test_new_token_error',
    'quic_server_test_handshake_done_error',
    'quic_server_test_newconnectionid_error',
    'quic_server_test_max_limit_error',
]

field_error_test = [
    'quic_server_test_blocked_streams_maxstream_error',
    'quic_server_test_retirecoid_error',
    'quic_server_test_stream_limit_error',
    'quic_server_test_newcoid_length_error',
    'quic_server_test_newcoid_rtp_error',
    'quic_server_test_max_error',
]


def write_header_results(writer,title):
    writer.write('\\begin{table}[h!]\n')
    writer.write('\centering\n')
    writer.write('\label{tab:server-summary-'+ title + '}\n')
    writer.write('\scriptsize\n')
    writer.write('\\resizebox{\\textwidth}{!}{%\n')
    writer.write('\\begin{tabular}{|l|ccccccc|}\n')
    writer.write('\hline\n')
    writer.write('\multicolumn{1}{|c|}{} & \multicolumn{1}{c|}{quinn} & \multicolumn{1}{c|}{mvfst} & \multicolumn{1}{c|}{picoquic} & \multicolumn{1}{c|}{quic-go} & \multicolumn{1}{c|}{aioquic} & \multicolumn{1}{c|}{quant} & quiche \\\\ \hline  \n')

def write_footer_results(writer, title):
    writer.write('\end{tabular}%\n')
    writer.write('}\n')
    writer.write('\caption{Server - '+ title +'}\n')
    writer.write('\end{table}\n')

def write_summary_line(writer , testname, results, line):
    testname = testname.replace("quic_server_test_", "")
    testname = testname.replace("_",'\_')

    implementation = ["quinn", "mvfst", "picoquic", "quic-go", "aioquic", "quant", "quiche"]

    writer.write(testname + ' & \n')

    for impl in implementation:
        color = ""
        if results[impl] > 90:
            color = "009933" # Dark green
        elif results[impl] > 75 and results[impl] <= 90 :
            color = "00ff55" # green
        elif results[impl] > 60 and results[impl] <= 75 :
            color = "ffff00" # yellow
        elif results[impl] > 40 and results[impl] <= 60 :
            color = "ffbb33" # orange
        elif results[impl] > 25 and results[impl] <= 40 :
            color = "e69900" # dark orange
        elif results[impl] > 10 and results[impl] <= 25 :
            color = "ff6666" # light red
        elif results[impl] == -1 :
            color = "ffffff" # white
        else:
            color = "FE0000" # red

        if impl != "quiche":
            writer.write('\cellcolor[HTML]{'+ color +'} '+ str(results[impl]) +'\%  & \n')
        else:
            writer.write('\cellcolor[HTML]{'+ color +'} '+ str(results[impl]) +'\%  \\\\ '+ line +'\n')


def write_header_tex(writer,implementation, title):
    writer.write('\\begin{table}[h!]\n')
    writer.write('\centering\n')
    writer.write('\label{tab:server-'+ implementation + '}\n')
    writer.write('\scriptsize\n')
    writer.write('\caption{\lstinline{' + implementation +'} server errors - '+ title +'}')
    writer.write('\\resizebox{\\textwidth}{!}{%\n')
    writer.write('\\begin{tabular}{llr}\n')
    #writer.write('\hline\n')
    writer.write('\multicolumn{1}{c}{\\textbf{Test}} & \multicolumn{1}{c}{\\textbf{Error code}} \multicolumn{1}{c}{\\textbf{\#}} \\\\ \n')

def write_footer_tex(writer):
    writer.write('\end{tabular}%\n')
    writer.write('}\n')
    writer.write('\end{table}\n')

def write_result(writer, nb):
    writer.write('  &       & \\textbf{' + str(nb) + '/100}      \\\\ \n')
    writer.write('   &       &   \\\\ \n')

def write_result_no_error(writer, testname):
    testname = testname.replace("_",'\_')
    writer.write(testname +' &       & \\textbf{0.0/100}      \\\\ \n')
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
    train_df = train_df[train_df.TestName != 'quic_server_test_tp_limit_acticoid_error']
    train_df = train_df[train_df.TestName != 'quic_server_test_stop_sending_error']
    train_df = train_df[train_df.TestName != 'quic_server_test_crypto_limit_error']
    train_df = train_df[train_df.TestName != 'quic_server_test_newcoid_zero_error']
    return train_df

def extract_implementation(train_df):
    server_name = train_df.Implementation.unique()
    print(server_name)
    #pd.options.display.max_colwidth = 200
    #print(train_df[train_df["Implementation"].isna()]["OutputFile"])
    servers = []
    for s in server_name:
        if "cargo run --manifest-path=tools/apps/Cargo.toml" in s:
            servers.append("quiche")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quiche")
        elif "cargo run --example server" in s or "cargo run -vv --example server" in s:
            servers.append("quinn")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quinn")
        elif "./http_server" in s:
            servers.append("lsquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "lsquic")
        elif "./picoquicdemo" in s:
            servers.append("picoquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "picoquic")
        elif "./server -d" in s or "/home/chris/TVOQE_UPGRADE_27/quic/quant/Debug/bin/server" in s:
            servers.append("quant")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quant")
        elif "./server -c" in s or "./server -p 4443 127.0.0.1" in s:
            servers.append("quic-go")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quic-go")
        elif "python3" in s:
            servers.append("aioquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "aioquic")
        elif "echo" in s:
            servers.append("mvfst")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "mvfst")
    return servers

def extract_file_name(path):
    splited = path.split("/")
    filename = splited[-2]+ "/" + splited[-1] 
    return filename

def extract_error(foldername, train_df):
    for i, row in train_df.iterrows():
        if row["isPass"] == 0.0:
            resultFile =  foldername +"temp/" + extract_file_name(row["OutputFile"])
            f = open(resultFile, "r")
            content = f.read()
            if row["Implementation"] == "quiche":
                #In fact we only do unidirectionnal with quiche
                if "require conn_total_data(the_cid) > 0;" in content and "stream.handle" in content: 
                    train_df.loc[i, "ErrorIEV"] = "No error"
                    train_df.loc[i, "isPass"] = 1.0
                    continue
            if "frame.connection_close:" in content and not "quic_server_test_connection_close" in resultFile:
                # Extract CONNECTION_CLOSE frame
                start_index = content.rfind("frame.connection_close:")
                end_index = content.find(",",start_index)
                train_df.loc[i, "ErrorIEV"] = content[start_index:end_index+1].replace(",","") + "}"

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
            elif "test_completed" in content:
                 train_df.loc[i, "ErrorIEV"] = 'No Error'
                 train_df.loc[i, "isPass"] = 1.0
            elif "quic_server_test_new_token_error.ivy: line 281" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_protocol_violation;'
            elif "quic_server_test_max_limit_error.ivy: line 527" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_server_test_max_error.ivy: line 517" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_server_test_newcoid_rtp_error.ivy: line" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_server_test_newcoid_length_error.ivy: line 527" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_server_test_unkown.ivy: line 571" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_server_test_newconnectionid_error.ivy: line 504" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
            elif "quic_server_test_retirecoid_error.ivy: line 507" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_protocol_violation;'
            elif "quic_server_test_unkown_tp.ivy: line 568" in content:
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
                start_index = content.find('assumption_failed(""')
                end_index = content.find('"")',start_index)
                c = content[start_index:end_index+1]
                c = c.replace('assumption_failed(""',"")
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
            elif "timeout+< show_pstats" in row["ErrorIEV"]:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            elif "timeout+< show_pstats" in content:
                train_df.loc[i, "ErrorIEV"] = "Timeout"
            #elif content.count("tls_recv_event") > 15:
                # Approximation but can induce false positive
            #    train_df.loc[i, "ErrorIEV"] = "Handshake not completed"
        else:
            train_df.loc[i, "ErrorIEV"] = "No Error"
    for i, row in train_df.iterrows():
        if row["isPass"] == 0.0:
            train_df.loc[i, "ErrorIEV"]  = train_df.loc[i, "ErrorIEV"].replace("\n","").replace("    ","").replace("\"","") #.replace('}',"\}").replace('{',"\{")

def multiple_output(train_df):
    for i, row in train_df.iterrows():
        if row['TestName'] == 'quic_server_test_token_error':
            if row["ErrorIEV"] == "Handshake not completed" or row["ErrorIEV"] == "Timeout":
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
        elif row['TestName'] == 'quic_server_test_double_tp_error':
            if row["ErrorIEV"] == "Handshake not completed" or row["ErrorIEV"] == "Timeout":
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
        elif row['TestName'] == 'quic_server_test_unkown':
            if "frame.connection_close:{err_code:0x7}" in row["ErrorIEV"]:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"
        elif row['TestName'] == 'quic_server_test_no_icid':
            if "frame.connection_close:{err_code:0x8}" in row["ErrorIEV"]:
                train_df.loc[i, "isPass"] = 1.0
                train_df.loc[i, "ErrorIEV"] = "No Error"

def pprint(str):
    print("="*50)
    print("==== " + str)
    print("="*50)

def get_errors(train_df, tests, f):
    servers = train_df.Implementation.unique() 
    tests = train_df.TestName.unique() 
    title = "Full"
    for i in servers:
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
                    if not error == "No Error" :
                        if first:
                            write_test_line(f,t,error, errors[error])
                            first = False
                        else:
                            write_line(f,error, errors[error])
                print(i)
                if (cnt - total[i+t]) > 0:
                    write_result(f, cnt - total[i+t])
                else:
                    write_result_no_error(f, t)
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
		    "quiche":-1
		}
            if t != last:
                write_summary_line(f, t, summary, "\cline{1-1}")
            else:
                write_summary_line(f, t, summary, "\hline")
            continue   
        summary = {
            "quinn":0,
            "mvfst":0,
            "picoquic":0,
            "quic-go":0, 
            "aioquic":0, 
            "quant":0, 
            "quiche":0
        }
        for i in servers:
            subsubdf = subdf.loc[subdf['Implementation'] == i]
            errors = {}
            cnt = 0
            for _ , row in subsubdf.iterrows():
                err = row['ErrorIEV']
                if err == "No Error":
                    summary[i] = summary[i] + 1
        if t != last:
            write_summary_line(f, t, summary, "\cline{1-1}")
        else:
            write_summary_line(f, t, summary, "\hline")
    write_footer_results(f,title)

def get_errors_split(train_df, tests,title, total, f):
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
		    "quiche":-1
		}
            if t != last:
                write_summary_line(f, t, summary, "\cline{1-1}")
            else:
                write_summary_line(f, t, summary, "\hline")
            continue   
        summary = {
            "quinn":0,
            "mvfst":0,
            "picoquic":0,
            "quic-go":0, 
            "aioquic":0, 
            "quant":0, 
            "quiche":0
        }
        for i in servers:
            subsubdf = subdf.loc[subdf['Implementation'] == i]
            errors = {}
            cnt = 0
            for _ , row in subsubdf.iterrows():
                err = row['ErrorIEV']
                if err == "No Error":
                    summary[i] = summary[i] + 1
        if t != last:
            write_summary_line(f, t, summary, "\cline{1-1}")
        else:
            write_summary_line(f, t, summary, "\hline")
    write_footer_results(f,title)




foldername = "/home/chris/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/errors/server/local/server-go-errors/" 
#foldername = "/home/chris/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/"
csv_name = "May-26-2021.csv"

train_df = pd.read_csv(foldername + csv_name, index_col=0)
print(train_df.head())

# Filter test not working
pprint("Filter test not working")
train_df = filter_test(train_df)
train_df = train_df[train_df["Implementation"].notna()]
# Replace with correct implementation name
pprint("Correct implementation name")
servers = extract_implementation(train_df)

print(servers)
print(train_df.head()) 

# Find the most appropriate error name
pprint("Find the most appropriate error name")
extract_error(foldername, train_df)
        
print(train_df.head())

# Preprocess multiple valid output possible
pprint("Preprocess multiple valid output possible")
multiple_output(train_df)

print(train_df.head())

# Get total per implementation & test
pprint("Get total per implementation & test")
tests = train_df.TestName.unique()
servers = train_df.Implementation.unique() 
print(tests)
print(servers)
total = {}
for t in tests: 
    subdf = train_df.loc[train_df['TestName'] == t]
    for s in servers: 
        ssubdf = subdf.loc[subdf['Implementation'] == s]
        total[s+t] = ssubdf["isPass"].sum()
        print(t)
        print(s)
        print(len(ssubdf.index))
        print(total[s+t])
        print()

outputFile = csv_name.replace("csv","txt")
f = open(outputFile, "w")
# Get total per implementation & test
pprint("Get errors count")

get_errors(train_df, total, f)

get_errors_split(train_df, global_test, "Global test", total, f)
get_errors_split(train_df, unk_test, "Unknown situation", total, f)
get_errors_split(train_df, tp_error_test, "Transport parameter errors tests", total, f)
get_errors_split(train_df, field_error_test, "Invalid fields frames errors tests", total, f)
get_errors_split(train_df, prot_error_test, "Protocol violation errors tests", total, f)


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
            elif "quic_server_test_new_token_error.ivy: line 281" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_protocol_violation;'
            elif "quic_server_test_max_limit_error.ivy: line 527" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_server_test_max_error.ivy: line 517" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_stream_limit_error;'
            elif "quic_server_test_newcoid_rtp_error.ivy: line" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_server_test_newcoid_length_error.ivy: line 527" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_frame_encoding_error;'
            elif "quic_server_test_newconnectionid_error.ivy: line 504" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require conn_total_data(the_cid) > 0;'
            elif "quic_server_test_retirecoid_error.ivy: line 507" in content:
                 train_df.loc[i, "ErrorIEV"] = 'require is_protocol_violation;'
            elif "quic_server_test_unkown_tp.ivy: line 568" in content:
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


