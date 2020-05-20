# import pathlib
import json
import redis
import hashlib
import pandas as pd
from model import IncCFModel as model

import os
from lib import mysql
from lib import bootstrap as boot

project_id = boot.project_id
root = boot.data_path
cluster = boot.cluster
config = boot.config

ratings_cols = ['user_id', 'content_id', 'rating']
ratings = pd.read_csv(root+'ratings_article_inc.csv', sep=',', names=ratings_cols, encoding='utf-8')
# print(ratings)

keywords_cols = ['content_id', 'id', 'title', 'keywords']
contents = pd.read_csv(root+'keywords_article_inc.csv', sep=',', names=keywords_cols, encoding='utf-8')
model.contents = contents;
# print(contents)
genre_cols = ['genre', 'count']
genre = pd.read_csv(root+'genre_article_inc.csv', sep=' ', names=genre_cols, encoding='utf-8')
model.genre = genre
# print(genre.get_value(index=67, col='genre'))
# print(genre.shape[0])

ratings["rating"] = ratings["rating"].apply(lambda x: float('%.3f' % x))

# build model
reg_model = model.build_increment_model(
    ratings, regularization_coeff=0.1, gravity_coeff=1.0, embedding_dim=config['embedding_dim'],
    init_stddev=.05)

U = pd.read_csv(root+"U.csv", sep=',').values
V = pd.read_csv(root+"V_inc.csv", sep=',').values
reg_model.embeddings['user_id'] = U
reg_model.embeddings['content_id'] = V

# save user recomendations to  to redis
mysql_conf = config['mysql']
# print(mysql_conf)
db = mysql.Mysql(config=mysql_conf)
db.connect()
info = db.select("select * from project where id = 1")
# print(info)
project_conf = json.loads(info['config'])
for k,v in project_conf['pika'].items():
    if 'master' in v and v['master'] == 1:
        rdscf = v['servers']
        break;
print(k, rdscf)
if rdscf is None:
    print("redis config not null!")
    exit()
# print(rdscf)
# exit()
rds = redis.Redis(host=rdscf['host'], port=rdscf['port'], password=rdscf['pass'], decode_responses=True)

if 'expire' in config:
    expire = 86400*config['expire']
else:
    expire = 86400*3

if 'topk' in config:
    topk = config['topk']
else:
    topk = 100
prefix = info['prefix']
str_key = prefix + '_string_'
zset_key = prefix + '_zset_'
for index, row in genre.iterrows():
    str_md5 = hashlib.md5(row['genre'].encode(encoding='UTF-8')).hexdigest()[0:16]
    # print(str_md5)
    rds.set(str_key + str_md5, row['count'], px=expire*1000)
    rl = model.user_recommendations(reg_model, model.DOT, exclude_rated=False, k=row['count'], id=index)
    rl = rl.merge(contents, on="content_id", how="left")
    # print(rl)
    dt = {int(rl.loc[i, 'id']):rl.loc[i, 'dot score']+4.5 for i in rl.index}
    # print(dt)
    rds.zadd(zset_key + str_md5, dt)
    rds.expire(zset_key + str_md5, expire)
    c = rds.zcount(zset_key + str_md5, "-inf", "+inf")
    if c > topk:
        rds.zremrangebyrank(zset_key + str_md5, 0, c-topk-1)
    # exit()