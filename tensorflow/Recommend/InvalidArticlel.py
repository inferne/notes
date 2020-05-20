import pathlib
import sys, getopt
import json
import redis
import hashlib
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

dsql = config['data']['mysql']['invalid_article_sql']
# dsql = "select article_id from game_article where project_id = 2 and status != 1 and article_id = 2843"
sobj = re.search("select (.*) from", dsql, re.M|re.I)
fi = re.sub('[\'\"]', "", sobj.group(1)).split(",")
print(fi)
if len(fi) == 0:
    print("fields number not matching!")
    exit()

project_conf = json.loads(info['config'])
for k,v in project_conf['pika'].items():
    if 'master' in v and v['master'] == 1:
        rdscf = v['servers']
        break;
print(k, rdscf)
if rdscf is None:
    print("redis config not null!")
    exit()

rds = redis.Redis(host=rdscf['host'], port=rdscf['port'], password=rdscf['pass'], decode_responses=True)

db2 = mysql.Mysql(config=config['data']['mysql'])
db2.connect()

sql = dsql
print(sql)
record = db2.select_list(sql)
# print(record)

prefix = info['prefix']
str_key = prefix + '_string_'
zset_key = prefix + '_zset_'
idskey = {}
if record:
    data_root = pathlib.Path(root)
    contents_keywords = (data_root/"keywords_article.csv").open(encoding='utf-8').readlines()
    contents_keywords = [line[:-1].split(',') for line in contents_keywords]
    for x in contents_keywords:
        idskey[x[1]] = x[3].split(' ')
    # print(idskey['2843'])
    for x in record:
        print(x[fi[0]])
        if str(x[fi[0]]) in idskey:
            print(idskey[str(x[fi[0]])])
            for k in idskey[str(x[fi[0]])]:
                str_md5 = hashlib.md5(k.encode(encoding='UTF-8')).hexdigest()[0:16]
                print(str_md5)
                c = rds.get(str_key + str_md5)
                if c is not None:
                    # delete ids
                    rds.set(str_key + str_md5, --c, px=expire*1000)
                    rds.zrem(zset_key + str_md5, x[fi[0]])
            # break