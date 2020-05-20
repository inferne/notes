import sys, getopt
import time
import os
import socket
import json

try:
    argv = sys.argv[1:]
    opts, args = getopt.getopt(argv, "p:d:")
    for opt, arg in opts:
        if opt == '-p':
            project_id = arg
        elif opt == '-d':
            d = arg
except getpot.GetoptError:
    print(sys.argv[0] + ' -p <id>')
    sys.exit(2)

t = time.strftime("%Y%m%d", time.localtime())
data_path = './data/'+d+'/'+t+'/'
if os.path.exists(data_path) == False:
    os.makedirs(data_path, 0o755)

names = socket.gethostname().split('.')
cluster = "corp"
if len(names) > 2:
    cluster = names[2]

with open('config.json') as f:
    config = json.load(f)