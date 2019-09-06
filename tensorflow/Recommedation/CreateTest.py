import pathlib
import os
import codecs
import pandas as pd
import random as rd

data_root = pathlib.Path("./")

#with codecs.open("users.csv", 'w') as f:
#    i = 1
#    while i <= 100:
#        f.write(str(i) + '\n')
#        i+=1
content_cols = ['content_id', 'title', 'channel', 'keywords', 'new_keywords']
content = pd.read_csv('content1000.csv', sep=',', names=content_cols, encoding='utf-8')

print(content.head())

#print(content['content_id'][1])

with codecs.open("data.csv", 'w') as f:
    i = 0
    while i < 10000:
        f.write(str(rd.randint(1, 100)) + "," + str(content['content_id'][rd.randint(0, 998)]) + ",1" + '\n')
        i+=1
