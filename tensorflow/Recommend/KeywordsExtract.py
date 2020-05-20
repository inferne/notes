import pathlib
import codecs
#import pandas as pd
from jieba import analyse
import re
import time
import os, sys

from lib import bootstrap as boot
root = boot.data_path

tfidf = analyse.extract_tags
analyse.set_stop_words('stopword.txt')

#content_cols = ['content_id', 'title', 'channel', 'keywords', 'new_keywords']
#contents = pd.read_csv('content1000.csv', sep=",", names=content_cols, encoding='utf-8')

data_root = pathlib.Path(root)

contents = (data_root/"export_article.csv").open(encoding='utf-8').readlines()
# print(contents[10])
contents = [line[:-1].split(',,,') for line in contents]
# print(contents[10])
# exit()
m=10
r1 = '<[^>]+>'
r2 = "[.!//_,$&%^*()<>+\"'?@#-|:~{}]+|[——！\\\\，。=？、：“”‘’《》【】￥……（）]+"
r3 = "[《》【】]"
g = {}
try:
    contents_keywords = (data_root/"keywords_article.csv").open(encoding='utf-8').readlines()
    contents_keywords = [line[:-1].split(',') for line in contents_keywords]
    contents_keywords = [line[3].split(' ') for line in contents_keywords]
except IOError:
    contents_keywords = []
# exit()
if len(contents_keywords) == 0:
    contents_keywords = []
    with codecs.open(root+"keywords_article.csv", 'w', 'utf-8') as f:
        i = 0
        for x in contents:
            if len(x) < 6:
                continue
            keywords = (re.sub(r3, "", x[3] + " " + x[4] + " ")).split(" ")
            keywords = list(filter(None, set(keywords)))

            text = x[1] + " " + x[1] + " " + x[2] + " " + x[5]
            text = re.sub(r1, "", text)
            text = re.sub(r2, " ", text)
            kw = tfidf(text, topK=m, withWeight=False, allowPOS=())
            
            for z in kw:
                if len(keywords) < m:
                    if z not in keywords:
                        keywords.append(z)
                else:
                    break;
            keywords = list(set(keywords))
            keywords = [keyword.lower() for keyword in keywords]
            # print(keywords)
            # exit()
            f.write(str(i) + "," + x[0] + ","  + x[1] + "," + " ".join(keywords) + '\n')
            contents_keywords.append(keywords)
            i+=1
            for y in keywords:
                if y not in g:
                    g[y] = 1
                else:
                    g[y] += 1
# print(contents_keywords[0])
# exit()
g = sorted(g.items(), key=lambda item:item[1], reverse=True)

try:
    genre = (data_root/"genre_article.csv").open(encoding='utf-8').readlines()
    genre = [line[:-1].split(' ') for line in genre]
    genre = [line[0] for line in genre]
except IOError:
    genre = []
if len(genre) == 0:
    with codecs.open(root+"genre_article.csv", 'w', 'utf-8') as f:
        i = 0;
        for x in g:
            if x[0] == 0 or x[0] == '':
                continue;

            # give up count 1
            if x[1] > 0:
                f.write(x[0] + " " + str(x[1]) + '\n')
                genre.append(x[0])
            i+=1
print(genre[0])
g = {}
i = 0
for x in genre:
    if x not in g:
        g[x] = i
        i+=1

# half-life period
hlp = len(contents_keywords)
i = 0
with codecs.open(root+'ratings_article.csv', 'w', 'utf-8') as f:
    for x in contents_keywords:
        r = float('%.4f' % (0.001 + 5 * i / hlp * i / hlp))
        for y in x:
            if y != '' and y in g:
                f.write(str(g[y]) + "," + str(i) + "," + str(r) + "\n")
        i+=1
