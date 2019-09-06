import pathlib
import os
import codecs

def save_token_to_int(data, file_path='./data/tags_token_results_int'):
    dict = {}
    i = 1
    for x in data:
        for y in x:
            if y not in dict:
                dict[y] = i
                i+=1
    print(len(dict))
    with codecs.open(file_path + "_dict", 'w') as f:
        f.write(str(dict))
    with codecs.open(file_path, 'w', 'utf-8') as f:
        for x in data:
            line = []
            for y in x:
                v = dict.get(y)
                line.append(str(v))
            f.write(" ".join(line) + '\n')

data_root = pathlib.Path("./data")
train_data = (data_root/"tags_token_results").open(encoding='utf-8').readlines()
train_data = [line[:-1].split(' ') for line in train_data]

save_token_to_int(train_data)
