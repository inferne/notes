import tensorflow as tf
from tensorflow import keras

import numpy as np
import pathlib
import os

def list_str2int(data):
    for i,v in enumerate(data):
        data[i] = int(v)
    return data

data_root = pathlib.Path("./data")

train_data = (data_root/"tags_token_results_int").open(encoding="utf-8").readlines()
train_data = [line[:-1].split(' ') for line in train_data]
train_labels = (data_root/"tags_token_results_tag").open(encoding="utf-8").readlines()
train_labels = list_str2int(train_labels)

print("Training entries:{}, labels: {}".format(len(train_data), len(train_labels)))
print(train_data[0], train_labels[0], type(train_labels[0]))
print(len(train_data[0]), len(train_data[1]))
#exit()
train_data = keras.preprocessing.sequence.pad_sequences(train_data,
                                                        value=0,
                                                        padding='post',
                                                        maxlen=64)
print(len(train_data[0]), len(train_data[1]))
print(train_data[0])

checkpoint_path = "training_1/cp-{epoch:04d}.ckpt"
checkpoint_dir = os.path.dirname(checkpoint_path)

cp_callback = tf.keras.callbacks.ModelCheckpoint(
    checkpoint_path, verbose=1, save_weights_only=True,
    period=1)

vocab_size = 380000
model = keras.Sequential()
model.add(keras.layers.Embedding(vocab_size, 16))
model.add(keras.layers.GlobalAveragePooling1D())
model.add(keras.layers.Dense(16, activation=tf.nn.relu))
model.add(keras.layers.Dense(1, activation=tf.nn.sigmoid))

model.summary()

model.compile(optimizer=tf.train.AdamOptimizer(),loss='binary_crossentropy',metrics=['accuracy'])

latest = tf.train.latest_checkpoint(checkpoint_dir)
idx = int(latest.split("-")[1].split(".")[0])
idx = 1
#print(latest, idx)
#exit()
if idx == 5:
    model.load_weights(latest)
else:
    x_val = train_data[:100000]
    partial_x_train = train_data[100000:]
    y_val = train_labels[:100000]
    partial_y_train = train_labels[100000:]

    print(len(partial_x_train), len(partial_y_train))

    history = model.fit(partial_x_train,
                        partial_y_train,
                        epochs=10, callbacks=[cp_callback],
                        batch_size=128,
                        validation_data=(x_val, y_val),
                        verbose=1)

    #model.save('my_model.h5')
#model.evaluate(x_val, y_val)
