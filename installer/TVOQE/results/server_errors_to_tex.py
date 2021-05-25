import threading
import multiprocessing
from datetime import date
import pandas as pd
import os
import scandir


def write_header_tex(writer,implementation):
    writer.write('\\begin{table}[h!]\n')
    writer.write('\centering\n')
    writer.write('\label{tab:server-'+ implementation + '}\n')
    writer.write('\scriptsize\n')
    writer.write('\\resizebox{\\textwidth}{!}{%\n')
    writer.write('\\begin{tabular}{llr}\n')
    writer.write('\hline\n')
    writer.write('\multicolumn{1}{c}{\\textbf{Test}} & \multicolumn{1}{c}{\\textbf{Error code}} \multicolumn{1}{c}{\\textbf{\#}} \\\\ \n')


def write_footer_tex(writer):
    writer.write('\end{tabular}%\n')
    writer.write('}\n')
    writer.write('\end{table}\n')

def write_result(writer, nb):
    writer.write('  &       & \\textbf{' + str(nb) + '/100}      \\\n')
    writer.write('   &       &   \\\\ \n')

def write_line(writer, error, nb):
    error = error.replace("_",'\_')
    error = error.replace("~","$\sim$")
    error = error.replace(">","\\textgreater")
    writer.write('                           & ' + error +    ' & ' +  str(nb)      + '  \\ \n')

def write_test_line(writer, testname, error, nb):
    error = error.replace("_",'\_')
    error = error.replace("~",'$\sim$')
    error = error.replace(">","\\textgreater")
    testname = testname.replace("_",'\_')
    writer.write(  testname +' & ' + error +' & '+ str(nb)   + '  \\\n')

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
    return train_df

def extract_implementation(train_df):
    server_name = train_df.Implementation.unique()
    servers = []
    for s in server_name:
        if "cargo run --manifest-path=tools/apps/Cargo.toml" in s:
            servers.append("quiche")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quiche")
        elif "cargo run --example server" in s:
            servers.append("quinn")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quinn")
        elif "./http_server" in s:
            servers.append("lsquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "lsquic")
        elif "./picoquicdemo" in s:
            servers.append("picoquic")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "picoquic")
        elif "./server -d" in s:
            servers.append("quant")
            train_df["Implementation"] = train_df["Implementation"].replace(s, "quant")
        elif "./server -c" in s:
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
    return path.split["/"][-1]

def extract_error(foldername, train_df):
    for i, row in train_df.iterrows():
        if row["isPass"] == 0.0:
            resultFile =  foldername + extract_file_name(row["OutputFile"])
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
                c = content[start_index:end_index-1]
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
            elif "timeout+< undecryptable_packet_" in content:
                train_df.loc[i, "ErrorIEV"] = "No decryption keys"
            elif content.count("tls_recv_event") > 15:
                # Approximation but can induce false positive
                train_df.loc[i, "ErrorIEV"] = "Handshake not completed"
        else:
            train_df.loc[i, "ErrorIEV"] = "No Error"
    for i, row in train_df.iterrows():
        if row["isPass"] == 0.0:
            train_df.loc[i, "ErrorIEV"]  = train_df.loc[i, "ErrorIEV"].replace("\n","").replace("    ","")

def multiple_output(train_df):
    for i, row in train_df.iterrows():
        if row['TestName'] == 'quic_server_test_token_error':
            if row["ErrorIEV"] == "Handshake not completed":
                train_df.loc[i, "isPass"] = 1.0
        elif row['TestName'] == 'quic_server_test_double_tp_error':
            if row["ErrorIEV"] == "Handshake not completed":
                train_df.loc[i, "isPass"] = 1.0
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
    print("\n==== " + str)
    print("="*50)


foldername = "/home/chris/Toward-verification-of-QUIC-extensions/installer/TVOQE/results/"
csv_name = ".csv"

train_df = pd.read_csv(foldername + csv_name, index_col=0)
print(train_df.head())

# Filter test not working
pprint("Filter test not working")
train_df = filter_test(train_df)

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
total = {}
for t in tests: 
    subdf = train_df.loc[train_df['TestName'] == t]
    for s in servers: 
        ssubdf = subdf.loc[subdf['Implementation'] == s]
        total[s] = ssubdf["isPass"].sum()
        print(t)
        print(s)
        print(len(ssubdf.index))
        print(total[s])
        print()

outputFile = csv_name.replace("csv","txt")
f = open(outputFile, "w")
# Get total per implementation & test
pprint("Get errors count")
for i in servers:
    subdf = train_df.loc[train_df['Implementation'] == i]
    write_header_tex(f,i)
    for t in tests:
        print(t)
        errors = {}
        subsubdf = subdf.loc[subdf['TestName'] == t]
        for i, row in subsubdf.iterrows():
            err = row['ErrorIEV']
            if err in errors:
                errors[err] = errors[err] + 1
            else :
                errors[err] = 1
        print(errors)
        first = True
        for error in errors:
            if first:
                write_test_line(f,t,error, errors[error])
                first = False
            else:
                write_line(f,error, errors[error])
        write_result(f, len(subsubdf.iterrows())-total[s])
    write_footer_tex(f)