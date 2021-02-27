from datetime import date
import pandas as pd
import os
import scandir

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

# List of available client's tests
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

frame = pd.DataFrame(
    columns=["Run", "Implementation", "Mode", "TestName", "Status", "ErrorIEV","OutputFile"])


def readlastline(filename):
    last = ""
    second_last = ""
    with open(filename, 'r') as f:
        lines = f.read().splitlines()
        if len(lines) > 0:
            last = lines[-1]
        if len(lines) > 1:
            second_last = lines[-2]
    return last, second_last


# "/home/chris/Toward-verification-of-QUIC-extensions/result/"
foldername = "/results/temp"
subfolders = [f.path for f in scandir.scandir(foldername) if f.is_dir()]
run = 0
for fol in subfolders:
    for file in os.listdir(fol):
        if file.endswith(".iev"):
            fullPath = os.path.join(fol, file)
            out = file.replace(".iev", ".out")
            mode = "client"
            test_name = ""
            match = ""
            if "server" in file:
                mode = "server"
                for n in server_tests:
                    if n in file:
                        test_name = file.replace('.iev', '')
                        break
                with open(os.path.join(fol, "res_server.txt"), "r") as f:
                    for li in f:
                        if "implementation command:" in li:
                            match = li.replace("implementation command:","")
                            break
            else:
                for n in client_tests:
                    if n in file:
                        test_name = file.replace('.iev', '')
                        break
                with open(os.path.join(fol, "res_client.txt"), "r") as f:
                    for li in f:
                        if "implementation command:" in li:
                            match = li.replace("implementation command:","")
                            break
            outPath = os.path.join(fol, out)
            err = file.replace(".iev", ".err")
            errPath = os.path.join(fol, err)
            with open(fullPath, "r") as f:
                last, second_last = readlastline(fullPath)
                if last in "test_completed\n":
                    frame = frame.append(
                        {"Run": run,
                         "Implementation":match,
                         "Mode": mode,
                         "TestName": test_name,
                         "isPass": True,
                         "ErrorIEV": "",
                         "NbPktSend":0, #TODO
                         "OutputFile": fullPath}, ignore_index=True)
                else:
                    frame = frame.append(
                        {"Run": run,
                         "Implementation":match, 
                         "Mode": mode,
                         "TestName": test_name,
                         "isPass": False,
                         "ErrorIEV": last+"+"+second_last,
                         "NbPktSend":0, #TODO
                         "OutputFile": fullPath}, ignore_index=True)
                run += 1
        


today = date.today()
# Month abbreviation, day and year
d4 = today.strftime("%b-%d-%Y")
print("d4 =", d4)
frame.to_csv(d4+'.csv', index=False)
