from pathlib import Path
import tensorflow as tf
from tensorflow.keras import layers, Model

def small_lstm(dim_in=40, filters=2, dim_hidden=20, dim_lstm_in=40, kernel_size=10):
    input_data = layers.Input(shape=(1, dim_in), name="data") 
    input_state = layers.Input(shape=(dim_hidden * 4,), name="h")

    x = layers.Reshape((dim_in, 1))(input_data)
    x = layers.Conv1D(filters=filters, kernel_size=kernel_size, activation='relu', data_format="channels_last", use_bias=False)(x)
    x = layers.MaxPooling1D(pool_size=2)(x)

    x = layers.Flatten()(x)
    x = layers.Dense(dim_lstm_in)(x)
    
    h1 = input_state[:, 0 : dim_hidden]
    c1 = input_state[:, dim_hidden : dim_hidden*2]
    h2 = input_state[:, dim_hidden*2 : dim_hidden*3]
    c2 = input_state[:, dim_hidden*3 : dim_hidden*4]

    x = layers.Reshape((1, dim_lstm_in))(x)
    x, h1_out, c1_out = layers.LSTM(dim_hidden, return_sequences=True, return_state=True, unroll=True)(
        x, initial_state=[h1, c1]
    )
    x, h2_out, c2_out = layers.LSTM(dim_hidden, return_sequences=True, return_state=True, unroll=True)(x, initial_state=[h2, c2])
    state_buffer_out = layers.Concatenate(axis=-1, name="state_buffer_out")([h1_out, c1_out, h2_out, c2_out])

    x = layers.Flatten()(x)
    logits = layers.Dense(3)(x)
    probs = layers.Softmax(axis=-1, name="prediction")(logits)

    model = Model(inputs=[input_data, input_state], outputs=[probs, state_buffer_out])
    return model


model = small_lstm()

model.compile(loss=['mean_squared_error', None], optimizer='adam')

input_dims = model.input_spec[0].shape[2]
state_dims = model.input_spec[1].shape[1]

_output = model.predict([tf.ones((1, 1, input_dims)), tf.zeros((1, state_dims))])

def representative_data_gen():
    # This will produce garbage quantizations obviously
    yield [tf.ones((1, 1, input_dims)), tf.zeros((1, state_dims))]

converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = representative_data_gen

converter.target_spec.supported_ops = [tf.lite.OpsSet.SELECT_TF_OPS, tf.lite.OpsSet.TFLITE_BUILTINS_INT8]

# This can sometimes fix some errors
# converter._experimental_lower_tensor_list_ops = False

converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8

model_quant = converter.convert()

Path('example_quant.tflite').write_bytes(model_quant)
print('Done')
