import pathlib
import codecs
#import pandas as pd

#content_cols = ['content_id', 'title', 'channel', 'keywords', 'new_keywords']
#contents = pd.read_csv('content1000.csv', sep=",", names=content_cols, encoding='utf-8')

data_root = pathlib.Path('./')

contents = (data_root/"content10000.csv").open(encoding='utf-8').readlines()
contents = [line[1:-2].split('","') for line in contents]
#print(contents[0])
g = {}
for x in contents:
    genre = (x[3] + " " + x[4]).split(" ")
    genre = list(set(genre))
    #print(x[2], x[3], x[4], genre)
    #exit()
    with codecs.open('keywords10000', 'a', 'utf-8') as f:
        f.write(" ".join(genre) + '\n')
    #exit()
    for y in genre:
        if y not in g:
            g[y] = 1
        else:
            g[y] += 1

#print(g, len(g))

g = sorted(g.items(), key=lambda item:item[1], reverse=True)

#print(g)

with codecs.open("genre10000", 'w', 'utf-8') as f:
    i = 0;
    for x in g:
        if x[0] == 0 or x[0] == '':
            continue;
        f.write(x[0] + " " + str(x[1]) + '\n')
        i+=1
        #if i >= 300:
        #    break;

