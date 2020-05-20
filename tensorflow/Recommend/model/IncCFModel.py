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
import time

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

def split_dataframe(df, holdout_fraction=0.1):
    test = df.sample(frac=holdout_fraction, replace=False)
    train = df[~df.index.isin(test.index)]
    return train, test

def build_rating_sparse_tensor(ratings_df, U_full, V_full):
    """
    Args:
      ratings_df: a pd.DataFrame with `user_id`, `movie_id` and `rating` columns.
    Returns:
      A tf.SparseTensor representing the ratings matrix.
    """
    indices = ratings_df[['user_id', 'content_id']].values
    values = ratings_df['rating'].values
    #print(users.shape[0], contents.shape[0])
    if len(U_full) > 0:
      return tf.SparseTensor(
        indices=indices,
        values=values,
        dense_shape=[len(U_full), contents.shape[0]])
    elif len(V_full) > 0 :
      return tf.SparseTensor(
        indices=indices,
        values=values,
        dense_shape=[genre.shape[0], len(V_full)])
    else:
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
      loss_vals = [pd.DataFrame(metrics_val) for metrics_val in metrics_vals]
      loss_vals = loss_vals[0].merge(loss_vals[1], left_index=True, right_index=True)
      # print(loss_vals)
      pd.DataFrame(loss_vals).to_csv(root+"loss_vals.csv", index=False, header=True)
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

def user_recommendations(model, measure=DOT, exclude_rated=False, k=6, id=67):
  USER_RATINGS = 1
  # id = 67
  if USER_RATINGS:
    # print(model.embeddings['user_id'])
    # print(model.embeddings['content_id'])
    scores = compute_scores(
        model.embeddings["user_id"][id], model.embeddings["content_id"], measure)
    # print(scores, len(scores))
    score_key = measure + ' score'
    df = pd.DataFrame({
        score_key: list(scores),
        'content_id': contents['content_id'],
    })
    if exclude_rated:
      # remove movies that are already rated
      rated_contents = ratings[ratings.user_id == id]["content_id"].values
      df = df[df.content_id.apply(lambda content_id: content_id not in rated_contents)]
    return df.sort_values([score_key], ascending=False).head(k)

def gravity(U, V):
  """Creates a gravity loss given two embedding matrices."""
  return 1. / (U.shape[0].value*V.shape[0].value) * tf.reduce_sum(
      tf.matmul(U, U, transpose_a=True) * tf.matmul(V, V, transpose_a=True))

def build_increment_model(
    ratings, embedding_dim=3, regularization_coeff=.1, gravity_coeff=1.,
    init_stddev=0.1, U_full=[], V_full=[]):
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
  # train_ratings, test_ratings = split_dataframe(ratings)
  train_ratings = ratings
  # SparseTensor representation of the train and test datasets.
  A_train = build_rating_sparse_tensor(train_ratings, U_full, V_full)
  # A_test = build_rating_sparse_tensor(test_ratings)
  if len(U_full) > 0:
    U = tf.constant(U_full, dtype="float32")
  else:
    U = tf.Variable(tf.random_normal(
      [A_train.dense_shape[0], embedding_dim], stddev=init_stddev))
  if len(V_full) > 0:
    V = tf.constant(V_full, dtype="float32")
  else:
    V = tf.Variable(tf.random_normal(
      [A_train.dense_shape[1], embedding_dim], stddev=init_stddev))

  # U_f = tf.constant(U_full)
  # V_f = tf.constant(V_full)

  error_train = sparse_mean_square_error(A_train, U, V)
  # error_test = sparse_mean_square_error(A_test, U, V)
  gravity_loss = gravity_coeff * gravity(U, V)
  regularization_loss = regularization_coeff * (
      tf.reduce_sum(U*U)/U.shape[0].value + tf.reduce_sum(V*V)/V.shape[0].value)
  total_loss = error_train + regularization_loss + gravity_loss
  losses = {
      'train_error_observed': error_train,
      # 'test_error_observed': error_test,
  }
  loss_components = {
      'observed_loss': error_train,
      'regularization_loss': regularization_loss,
      'gravity_loss': gravity_loss,
  }
  embeddings = {"user_id": U, "content_id": V}

  return CFModel(embeddings, total_loss, [losses, loss_components])