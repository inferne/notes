from __future__ import print_function

import numpy as np
import pandas as pd
import collections
from IPython import display
import sklearn
import sklearn.manifold
import tensorflow as tf
tf.logging.set_verbosity(tf.logging.ERROR)
from model import CFModel as model

import os
import time

from lib import bootstrap as boot
root = boot.data_path
model.root = root
config = boot.config

# Add some convenience functions to Pandas DataFrame.
pd.options.display.max_rows = 10
pd.options.display.float_format = '{:.3f}'.format
def mask(df, key, function):
  """Returns a filtered dataframe, by applying function to key"""
  return df[function(df[key])]

def flatten_cols(df):
  df.columns = [' '.join(col).strip() for col in df.columns.values]
  return df

pd.DataFrame.mask = mask
pd.DataFrame.flatten_cols = flatten_cols

ratings_cols = ['user_id', 'content_id', 'rating']
ratings = pd.read_csv(root+'ratings_article.csv', sep=',', names=ratings_cols, encoding='utf-8')
print(ratings)

keywords_cols = ['content_id', 'id', 'title', 'keywords']
contents = pd.read_csv(root+'keywords_article.csv', sep=',', names=keywords_cols, encoding='utf-8')
model.contents = contents
print(contents)
genre_cols = ['genre', 'count']
genre = pd.read_csv(root+'genre_article.csv', sep=' ', names=genre_cols, encoding='utf-8')
model.genre = genre
print(genre.get_value(index=67, col='genre'))
# print(genre.shape[0])

ratings["rating"] = ratings["rating"].apply(lambda x: float('%.3f' % x))

contents_ratings = contents.merge(
    ratings
    .groupby('content_id', as_index=False)
    .agg({'rating':['count', 'mean']})
    .flatten_cols(),
    on='content_id')

print(contents_ratings[['title', 'rating count', 'rating mean']]
.sort_values('rating count', ascending=False)
.head(10))

print(ratings)

reg_model = model.build_regularized_model(
    ratings, regularization_coeff=0.1, gravity_coeff=1.0, embedding_dim=config['embedding_dim'],
    init_stddev=.05)

try:
    U = pd.read_csv(root+"U.csv", sep=',').values
    V = pd.read_csv(root+"V.csv", sep=',').values
    reg_model.embeddings['user_id'] = U
    reg_model.embeddings['content_id'] = V
except IOError:
    reg_model.train(num_iterations=2000, learning_rate=20.)
    pd.DataFrame(reg_model.embeddings['user_id']).to_csv(root+'U.csv', index=False, header=True)
    pd.DataFrame(reg_model.embeddings['content_id']).to_csv(root+'V.csv', index=False, header=True)

# df.sort_values([score_key], ascending=False).to_csv('scores.csv', index=False, header=False)
print(genre.get_value(index=67, col='genre'))
result = model.user_recommendations(reg_model, model.DOT, exclude_rated=False, k=10)
result = result.merge(contents, on="content_id")
print(result)
