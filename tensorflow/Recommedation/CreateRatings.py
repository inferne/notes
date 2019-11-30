import pathlib
import codecs
#import pandas as pd

#content_cols = ['content_id', 'title', 'channel', 'keywords', 'new_keywords']
#contents = pd.read_csv('content1000.csv', sep=",", names=content_cols, encoding='utf-8')

data_root = pathlib.Path('./')

keywords = (data_root/"keywords10000").open(encoding='utf-8').readlines()
keywords = [line[0:-1].split(' ') for line in keywords]
print(keywords[0])
genre = (data_root/"genre10000").open(encoding='utf-8').readlines()
genre = [line[0:-1].split(' ') for line in genre]
print(genre[0])

g = {}
i = 0
for x in genre:
    if x[0] not in g:
        g[x[0]] = i
        i+=1

i = 0
for x in keywords:
    for y in x:
        if y != '':
            with codecs.open('ratings10000.csv', 'a', 'utf-8') as f:
                f.write(str(g[y]) + "," + str(i) + ",1\n")
    i+=1


# g = {}
# for x in contents:
#     genre = (x[3] + " " + x[4]).split(" ")
#     genre = list(set(genre))
#     #print(x[2], x[3], x[4], genre)
#     #exit()
#     with codecs.open('keywords', 'a', 'utf-8') as f:
#         f.write(" ".join(genre) + '\n')
#     #exit()
#     for y in genre:
#         if y not in g:
#             g[y] = 1
#         else:
#             g[y] += 1

# #print(g, len(g))

# g = sorted(g.items(), key=lambda item:item[1], reverse=True)

# #print(g)

# with codecs.open("genre", 'w', 'utf-8') as f:
#     i = 0;
#     for x in g:
#         if x[0] == 0 or x[0] == '':
#             continue;
#         f.write(x[0] + " " + str(x[1]) + '\n')
#         i+=1
#         #if i >= 300:
#         #    break;

