from ai_edge_litert.interpreter import Interpreter
import tensorflow as tf

interpreter = Interpreter(model_path='example_quant.tflite')
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()


input_shape = input_details[0]['shape']
input = 3 * tf.ones(input_shape, dtype=tf.int8)

hidden_shape = input_details[1]['shape']
hidden = tf.ones(hidden_shape, dtype=tf.int8)

interpreter.set_tensor(0, input)
interpreter.set_tensor(1, hidden)

interpreter.invoke()

print('output:', interpreter.get_tensor(output_details[0]['index']))
