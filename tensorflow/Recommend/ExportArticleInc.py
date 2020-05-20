import sys, getopt
import json
import codecs
import time
import os
import re

from lib import mysql
from lib import bootstrap as boot

project_id = boot.project_id
root = boot.data_path

with open('config.json') as f:
    #rdscf = json.load(f)['redis']
    mysql_conf = json.load(f)['mysql']
# print(mysql_conf)
db = mysql.Mysql(config=mysql_conf)
db.connect()
info = db.select("select * from project where id = " + project_id)

config = json.loads(info['config'])

dsql = config['data']['mysql']['sql']
sobj = re.search("select (.*) from", dsql, re.M|re.I)
fi = re.sub('[\'\"]', "", sobj.group(1)).split(",")
print(fi)
if len(fi) != 6:
    print("fields number not matching!")
    exit()

db2 = mysql.Mysql(config=config['data']['mysql'])
db2.connect()
flag = 1
max_id = 0
pagesize = 1000
with codecs.open(root+"export_article_inc.csv", "w", "utf-8") as f:
    while (flag == 1):
        sql = dsql + " and " + fi[0] + " > " + str(max_id) + " order by " + fi[0] + " desc limit 0," + str(pagesize)
        print(sql)
        record = db2.select_list(sql)
        if record:
            print(len(record))
            if len(record) == pagesize:
                max_id = record[pagesize-1][fi[0]]
            else:
                flag = 0
            for x in reversed(record):
                detail = re.sub('[\r\n]', "", x[fi[5]])
                for y in range(1, 5):
                    if x[fi[y]] == fi[y]:
                        x[fi[y]] = ""
                    else:
                        x[fi[y]] = " ".join(x[fi[y]].split(","))
                # exit()
                f.write(str(x[fi[0]]) + ",,," + x[fi[1]] + ",,," + x[fi[2]] + ",,," + x[fi[3]] + ",,," + x[fi[4]] + ",,," + detail + '\n')
        else:
            flag = 0
        exit()