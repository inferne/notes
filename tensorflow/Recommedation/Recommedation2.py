from __future__ import print_function

import numpy as np
import pandas as pd
import collections
from IPython import display
import sklearn
import sklearn.manifold
import tensorflow as tf
tf.logging.set_verbosity(tf.logging.ERROR)

import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

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

# users_cols = ['user_id']
# users = pd.read_csv('users.csv', sep=',', names=users_cols, encoding='utf-8')

ratings_cols = ['user_id', 'content_id', 'rating']
ratings = pd.read_csv('ratings10000.csv', sep=',', names=ratings_cols, encoding='utf-8')
print(ratings)

contents_cols = ['content_id', 'title', 'channel', 'keywords', 'new_keywords']
contents = pd.read_csv('content10000.csv', sep=',', names=contents_cols, encoding='utf-8')
print(contents)
keywords_cols = ['keywords']
contents['keywords'] = pd.read_csv('keywords10000', sep=',', names=keywords_cols, encoding='utf-8')
print(contents)
genre_cols = ['genre', 'count']
genre = pd.read_csv('genre10000', sep=' ', names=genre_cols, encoding='utf-8')
print(genre.get_value(index=67, col='genre'))
# print(genre.shape[0])
# exit()
def dict_content_id(data):
    dict = {}
    i = 0
    for x in data:
        if x not in dict:
            dict[x] = i
            i+=1
    return dict

dict = dict_content_id(contents['content_id'])
#print(dict)
# users['user_id'] = users['user_id'].apply(lambda x: str(x-1))
#contents['content_id'] = conversion_content_id(dict, contents['content_id'])
contents['content_id'] = contents['content_id'].apply(lambda x: dict.get(x))
# ratings['user_id'] = ratings['user_id'].apply(lambda x: str(x-1))
#ratings['content_id'] = conversion_content_id(dict, ratings['content_id'])
# ratings['content_id'] = ratings['content_id'].apply(lambda x: dict.get(x))
ratings["rating"] = ratings["rating"].apply(lambda x: float(x))
print(contents)
# exit()
contentlens = ratings.merge(contents, on='content_id')

def split_dataframe(df, holdout_fraction=0.1):
    test = df.sample(frac=holdout_fraction, replace=False)
    train = df[~df.index.isin(test.index)]
    return train, test

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

def build_rating_sparse_tensor(ratings_df):
    """
    Args:
      ratings_df: a pd.DataFrame with `user_id`, `movie_id` and `rating` columns.
    Returns:
      A tf.SparseTensor representing the ratings matrix.
    """
    indices = ratings_df[['user_id', 'content_id']].values
    values = ratings_df['rating'].values
    #print(users.shape[0], contents.shape[0])
    return tf.SparseTensor(
        indices=indices,
        values=values,
        dense_shape=[genre.shape[0], contents.shape[0]])


def sparse_mean_square_error(sparse_ratings, user_embeddings, content_embeddings):
    """
    Args:
      sparse_ratings: A SparseTensor rating matrix, of dense_shape [N, M]
      user_embeddings: A dense Tensor U of shape [N, k] where k is the embedding
        dimension, such that U_i is the embedding of user i.
      movie_embeddings: A dense Tensor V of shape [M, k] where k is the embedding
        dimension, such that V_j is the embedding of movie j.
    Returns:
      A scalar Tensor representing the MSE between the true ratings and the
        model's predictions.
    """
    #print(user_embeddings, content_embeddings, sparse_ratings.indices)
    predictions = tf.gather_nd(
        tf.matmul(user_embeddings, content_embeddings, transpose_b=True),
        sparse_ratings.indices)
    loss = tf.losses.mean_squared_error(sparse_ratings.values, predictions)
    return loss

#@title Alternate Solution
#def sparse_mean_square_error(sparse_ratings, user_embeddings, movie_embeddings):
#  predictions = tf.reduce_sum(
#      tf.gather(user_embeddings, sparse_ratings.indices[:, 0]) *
#      tf.gather(movie_embeddings, sparse_ratings.indices[:, 1]),
#      axis=1)
#  loss = tf.losses.mean_squared_error(sparse_ratings.values, predictions)
#  return loss

# @title CFModel helper class (run this cell)
class CFModel(object):
  """Simple class that represents a collaborative filtering model"""
  def __init__(self, embedding_vars, loss, metrics=None):
    """Initializes a CFModel.
    Args:
      embedding_vars: A dictionary of tf.Variables.
      loss: A float Tensor. The loss to optimize.
      metrics: optional list of dictionaries of Tensors. The metrics in each
        dictionary will be plotted in a separate figure during training.
    """
    self._embedding_vars = embedding_vars
    self._loss = loss
    self._metrics = metrics
    self._embeddings = {k: None for k in embedding_vars}
    self._session = None

  @property
  def embeddings(self):
    """The embeddings dictionary."""
    return self._embeddings

  def train(self, num_iterations=100, learning_rate=1.0, plot_results=False,
            optimizer=tf.train.GradientDescentOptimizer):
    """Trains the model.
    Args:
      iterations: number of iterations to run.
      learning_rate: optimizer learning rate.
      plot_results: whether to plot the results at the end of training.
      optimizer: the optimizer to use. Default to GradientDescentOptimizer.
    Returns:
      The metrics dictionary evaluated at the last iteration.
    """
    with self._loss.graph.as_default():
      opt = optimizer(learning_rate)
      train_op = opt.minimize(self._loss)
      local_init_op = tf.group(
          tf.variables_initializer(opt.variables()),
          tf.local_variables_initializer())
      if self._session is None:
        self._session = tf.Session()
        with self._session.as_default():
          self._session.run(tf.global_variables_initializer())
          self._session.run(tf.tables_initializer())
          tf.train.start_queue_runners()

    with self._session.as_default():
      local_init_op.run()
      iterations = []
      metrics = self._metrics or ({},)
      metrics_vals = [collections.defaultdict(list) for _ in self._metrics]
      #print(train_op, metrics)
      # Train and append results.
      for i in range(num_iterations + 1):
        _, results = self._session.run((train_op, metrics))
        if (i % 10 == 0) or i == num_iterations:
          print("\r iteration %d: " % i + ", ".join(
                ["%s=%f" % (k, v) for r in results for k, v in r.items()]),
                end='')
          iterations.append(i)
          for metric_val, result in zip(metrics_vals, results):
            for k, v in result.items():
              metric_val[k].append(v)
      print('\n')
      for k, v in self._embedding_vars.items():
        self._embeddings[k] = v.eval()

      if plot_results:
        # Plot the metrics.
        num_subplots = len(metrics)+1
        fig = plt.figure()
        fig.set_size_inches(num_subplots*10, 8)
        for i, metric_vals in enumerate(metrics_vals):
          ax = fig.add_subplot(1, num_subplots, i+1)
          for k, v in metric_vals.items():
            ax.plot(iterations, v, label=k)
          ax.set_xlim([1, num_iterations])
          ax.legend()
      return results

DOT = 'dot'
COSINE = 'cosine'
def compute_scores(query_embedding, item_embeddings, measure=DOT):
  """Computes the scores of the candidates given a query.
  Args:
    query_embedding: a vector of shape [k], representing the query embedding.
    item_embeddings: a matrix of shape [N, k], such that row i is the embedding
      of item i.
    measure: a string specifying the similarity measure to be used. Can be
      either DOT or COSINE.
  Returns:
    scores: a vector of shape [N], such that scores[i] is the score of item i.
  """
  u = query_embedding
  V = item_embeddings
  if measure == COSINE:
    V = V / np.linalg.norm(V, axis=1, keepdims=True)
    u = u / np.linalg.norm(u)
  scores = u.dot(V.T)
  return scores

def user_recommendations(model, measure=DOT, exclude_rated=False, k=6):
  USER_RATINGS = 1
  id = 67
  if USER_RATINGS:
    print(model.embeddings["user_id"])
    print(model.embeddings["content_id"])
    scores = compute_scores(
        model.embeddings["user_id"][id], model.embeddings["content_id"], measure)
    #print(scores, len(scores))
    score_key = measure + ' score'
    df = pd.DataFrame({
        score_key: list(scores),
        'content_id': contents['content_id'],
        'titles': contents['title'],
        'keywords': contents['keywords'],
    })
    if exclude_rated:
      # remove movies that are already rated
      rated_contents = ratings[ratings.user_id == id]["content_id"].values
      df = df[df.content_id.apply(lambda content_id: content_id not in rated_contents)]
    display.display(df.sort_values([score_key], ascending=False).head(k))

def content_neighbors(model, title_substring, measure=DOT, k=6):
  # Search for movie ids that match the given substring.
  ids =  contents[contents['title'].str.contains(title_substring)].index.values
  titles = contents.iloc[ids]['title'].values
  if len(titles) == 0:
    raise ValueError("Found no movies with title %s" % title_substring)
  print("Nearest neighbors of : %s." % titles[0])
  if len(titles) > 1:
    print("[Found more than one matching movie. Other candidates: {}]".format(
        ", ".join(titles[1:])))
  content_id = ids[0]
  scores = compute_scores(
      model.embeddings["content_id"][content_id], model.embeddings["content_id"],
      measure)
  score_key = measure + ' score'
  df = pd.DataFrame({
      score_key: list(scores),
      'titles': contents['title'],
      'keywords': contents['keywords']
  })
  # df.sort_values([score_key], ascending=False).to_csv('scores.csv', index=False, header=False)
  # exit()
  display.display(df.sort_values([score_key], ascending=False).head(k))

def gravity(U, V):
  """Creates a gravity loss given two embedding matrices."""
  return 1. / (U.shape[0].value*V.shape[0].value) * tf.reduce_sum(
      tf.matmul(U, U, transpose_a=True) * tf.matmul(V, V, transpose_a=True))

def build_regularized_model(
    ratings, embedding_dim=3, regularization_coeff=.1, gravity_coeff=1.,
    init_stddev=0.1):
  """
  Args:
    ratings: the DataFrame of movie ratings.
    embedding_dim: The dimension of the embedding space.
    regularization_coeff: The regularization coefficient lambda.
    gravity_coeff: The gravity regularization coefficient lambda_g.
  Returns:
    A CFModel object that uses a regularized loss.
  """
  # Split the ratings DataFrame into train and test.
  train_ratings, test_ratings = split_dataframe(ratings)
  # SparseTensor representation of the train and test datasets.
  A_train = build_rating_sparse_tensor(train_ratings)
  A_test = build_rating_sparse_tensor(test_ratings)
  U = tf.Variable(tf.random_normal(
      [A_train.dense_shape[0], embedding_dim], stddev=init_stddev))
  V = tf.Variable(tf.random_normal(
      [A_train.dense_shape[1], embedding_dim], stddev=init_stddev))

  error_train = sparse_mean_square_error(A_train, U, V)
  error_test = sparse_mean_square_error(A_test, U, V)
  gravity_loss = gravity_coeff * gravity(U, V)
  regularization_loss = regularization_coeff * (
      tf.reduce_sum(U*U)/U.shape[0].value + tf.reduce_sum(V*V)/V.shape[0].value)
  total_loss = error_train + regularization_loss + gravity_loss
  losses = {
      'train_error_observed': error_train,
      'test_error_observed': error_test,
  }
  loss_components = {
      'observed_loss': error_train,
      'regularization_loss': regularization_loss,
      'gravity_loss': gravity_loss,
  }
  embeddings = {"user_id": U, "content_id": V}

  return CFModel(embeddings, total_loss, [losses, loss_components])

reg_model = build_regularized_model(
    ratings, regularization_coeff=0.1, gravity_coeff=1.0, embedding_dim=35,
    init_stddev=.05)
reg_model.train(num_iterations=2000, learning_rate=20.)

user_recommendations(reg_model, DOT, exclude_rated=False, k=10)
# content_neighbors(reg_model, "赵本山", DOT)
# content_neighbors(reg_model, "赵本山", COSINE)


