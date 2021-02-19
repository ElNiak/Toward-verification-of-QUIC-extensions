import pandas as pd
import matplotlib.pyplot as plt

server_tests = [
    'quic_server_test_stream',
    'quic_server_test_handshake_done_error',
    'quic_server_test_reset_stream',
    'quic_server_test_connection_close',
    'quic_server_test_stop_sending',
    'quic_server_test_max',
    'quic_server_test_token_error',
    'quic_server_test_tp_error',
    'quic_server_test_double_tp_error',
    'quic_server_test_tp_acticoid_error',
    'quic_server_test_tp_limit_acticoid_error',
    'quic_server_test_blocked_streams_maxstream_error',
    'quic_server_test_retirecoid_error',
    'quic_server_test_newcoid_zero_error',
    'quic_server_test_accept_maxdata'
]

#List of available client's tests 
client_tests = [
    'quic_client_test_max',
    'quic_client_test_token_error',
    'quic_client_test_tp_error',
    'quic_client_test_double_tp_error',
    'quic_client_test_tp_acticoid_error',
    'quic_client_test_tp_limit_acticoid_error',
    'quic_client_test_blocked_streams_maxstream_error',
    'quic_client_test_retirecoid_error',
    'quic_client_test_newcoid_zero_error',
    'quic_client_test_accept_maxdata',
    'quic_client_test_tp_prefadd_error'
]

frame = pd.DataFrame(columns = ["Run","Mode","TestName","Status", "Error", "Output"])

def readlastline(filename):
    with open(filename, 'r') as f:
        lines = f.read().splitlines()
        last = lines[-1]
        second_last = lines[-2]
    return last, second_last

#"/home/chris/Toward-verification-of-QUIC-extensions/result/"
foldername =  "/results/temp" 
subfolders = [ f.path for f in os.scandir(foldername) if f.is_dir() ]
run = 0
for file in os.listdir(foldername+str(run)):
    if file.endswith(".iev"):
        fullPath = os.path.join(foldername, file)
        out = file.replace(".iev",".out")
        mode = "client"
        test_name = ""
        if "server" in file:
            mode = "server"
            for n in server_tests:
                if n in file:
                    test_name = file
                    break
        else:
           for n in client_tests:
                if n in file:
                    test_name = file
                    break 

	    outPath = os.path.join(foldername, out)
        err = file.replace(".iev", ".err")
	    errPath = os.path.join(foldername, err)
        with open(fullPath, "r") as f:
            last, second_last = readlastline(fullPath)
            if last in "test_completed\n":
                frame = frame.append(
                    {"Run":i, 
                    "Mode":mode,
                    "TestName":test_name, 
                    "isPass":True,
                    "Error":"",
                    "Output":fullPath}
                    , ignore_index=True)
            else
                print("fails")
                frame = frame.append(
                    {"Run":i, 
                    "Mode":mode,
                    "TestName":test_name, 
                    "isPass":False,
                    "Error": last+";"+second_last,
                    "Output":fullPath}
                    , ignore_index=True)
    run += 1

from datetime import date

today = date.today()
# Month abbreviation, day and year	
d4 = today.strftime("%b-%d-%Y")
print("d4 =", d4)
frame.to_csv(d4+'.zip', index=False,
          compression=compression_opts)