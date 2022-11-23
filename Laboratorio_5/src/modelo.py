'''Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Laboratorio 5
STM32/Arduino: GPIO, Giroscopio, comunicaciones, TinyML
Script de Python que genera modelo para cargar al microcontrolador
Ciclo: II-2022
'''

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import tensorflow as tf
import os
import sklearn as sk
import seaborn as sns
from sklearn import metrics

filename = "estacionario.csv"

df = pd.read_csv(filename)

index = range(1, len(df['Eje X']) + 1)

plt.rcParams["figure.figsize"] = (20,10)

# Gráfica de datos para un gesto 
plt.plot(index, df['Eje X'], 'g.', label='x', linestyle='solid', marker=',')
plt.plot(index, df['Eje Y'], 'b.', label='y', linestyle='solid', marker=',')
plt.plot(index, df['Eje Z'], 'r.', label='z', linestyle='solid', marker=',')
plt.title("Acceleration")
plt.xlabel("Sample #")
plt.ylabel("Acceleration (G)")
plt.legend()
plt.show()


# Entrenamiento y creación de red neuronal 

print(f"TensorFlow version = {tf.__version__}\n")

# Se crea una semilla aleatoria para crear el modelo 
SEED = 1337
np.random.seed(SEED)
tf.random.set_seed(SEED)

# Lista de gestos a utilizar en el modelo 
GESTURES = [
    "estacionario",
    "circulo",
    "golpe"
]

#Número de samples por gesto 
SAMPLES_PER_GESTURE = 119   

NUM_GESTURES = len(GESTURES)

# Matriz one-hot códificada utilizada para salidas del modelo 
ONE_HOT_ENCODED_GESTURES = np.eye(NUM_GESTURES)

# Vectores de salida y entrada 
inputs = []
outputs = []

# Se lee cada archivo csv y se rellenan los vectores de entrada y salida 
for gesture_index in range(NUM_GESTURES):
  gesture = GESTURES[gesture_index]
  print(f"Processing index {gesture_index} for gesture '{gesture}'.")
  
  output = ONE_HOT_ENCODED_GESTURES[gesture_index]
  
  df = pd.read_csv(gesture + ".csv")
  
  # Se calcula el número de recordings por gesto en el archivo 
  num_recordings = int(df.shape[0] / SAMPLES_PER_GESTURE)
  
  print(f"\tThere are {num_recordings} recordings of the {gesture} gesture.")

  for i in range(num_recordings):
    tensor = []
    for j in range(SAMPLES_PER_GESTURE):
      index = i * SAMPLES_PER_GESTURE + j
      # Normalización de los datos utilizados para entrenar el modelo entre 0 y 1 
      tensor += [
          (df['Eje X'][index] + 179) / 358,
          (df['Eje Y'][index] + 179) / 358,
          (df['Eje Z'][index] + 179) / 358
      ]
  # Se rellena los vecotres de entrada y salida 
  inputs.append(tensor)
  outputs.append(output)

# Se convierten las listas en arreglos de numpy
inputs = np.array(inputs)
outputs = np.array(outputs)

print("Data set parsing and preparation complete.")

# Se vuelve aleotrio el orden de las entradas para poder ser utilizados en entrenamiento, testeo y valicación 
num_inputs = len(inputs)
randomize = np.arange(num_inputs)
np.random.shuffle(randomize)

# Se cambian índices consecutivos (0,1,2,3,...) por índices aleatorios 
inputs = inputs[randomize]
outputs = outputs[randomize]

# Se separaon los datos en: entrenamiento, testeo and validación
TRAIN_SPLIT = int(0.6 * num_inputs)
TEST_SPLIT = int(0.2 * num_inputs + TRAIN_SPLIT)

inputs_train, inputs_test, inputs_validate = np.split(inputs, [TRAIN_SPLIT, TEST_SPLIT])
outputs_train, outputs_test, outputs_validate = np.split(outputs, [TRAIN_SPLIT, TEST_SPLIT])

print("Data set randomization and splitting complete.")

# Creación del modelo y entrenamiento 
model = tf.keras.Sequential()
model.add(tf.keras.layers.Dense(150, activation='relu')) # relu para rendimiuento 
model.add(tf.keras.layers.Dense(100, activation='relu'))
model.add(tf.keras.layers.Dense(50, activation='relu'))
model.add(tf.keras.layers.Dense(NUM_GESTURES, activation='softmax')) # softmax se usa porque se espera un gesto por entrada 
model.compile(optimizer='rmsprop', loss='mse', metrics=['mae'])
history = model.fit(inputs_train, outputs_train, epochs=600, batch_size=1, validation_data=(inputs_validate, outputs_validate))
model.summary()

# Aumento de tamaño de gráficos 
plt.rcParams["figure.figsize"] = (20,10)

# Se gráfica las pérdidas (loss)
loss = history.history['loss']
val_loss = history.history['val_loss']
epochs = range(1, len(loss) + 1)
plt.plot(epochs, loss, 'g.', label='Training loss')
plt.plot(epochs, val_loss, 'b', label='Validation loss')
plt.title('Training and validation loss')
plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.legend()
plt.show()

print(plt.rcParams["figure.figsize"])

# Se gráfica de nuevo las pérididas omitiendo un poco de la entrada
SKIP = 100
plt.plot(epochs[SKIP:], loss[SKIP:], 'g.', label='Training loss')
plt.plot(epochs[SKIP:], val_loss[SKIP:], 'b.', label='Validation loss')
plt.title('Training and validation loss')
plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.legend()
plt.show()


# Gráfica del mean absolute error
mae = history.history['mae']
val_mae = history.history['val_mae']
plt.plot(epochs[SKIP:], mae[SKIP:], 'g.', label='Training MAE')
plt.plot(epochs[SKIP:], val_mae[SKIP:], 'b.', label='Validation MAE')
plt.title('Training and validation mean absolute error')
plt.xlabel('Epochs')
plt.ylabel('MAE')
plt.legend()
plt.show()

# Se convierte a un modelo de Tensor Flow Lite 
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# Guarda el modelo en formeto .tflite
open("gesture_model.tflite", "wb").write(tflite_model)
  

basic_model_size = os.path.getsize("gesture_model.tflite")
print("Model is %d bytes" % basic_model_size)

#Prueba del modelo con datos para testeo 

# Se usa el modelo para predicir las entradas de testeo
predictions = model.predict(inputs_test)

# Se imprimen las predicciones y la salida esperada
print("predictions =\n", np.round(predictions, decimals=3))
print("actual =\n", outputs_test)

class_labels = GESTURES
max_test = np.argmax(outputs_test, axis=1)
max_predictions = np.argmax(predictions, axis=1)
confusion_matrix = metrics.confusion_matrix(max_test, max_predictions)
sns.heatmap(confusion_matrix, xticklabels = class_labels, yticklabels = class_labels, annot = True, linewidths = 0.1, fmt='d', cmap = 'YlGnBu')
plt.title("Confusion matrix", fontsize = 15)
plt.ylabel('True label')
plt.xlabel('Predicted label')
plt.show()

# Exportar como header file para poder usar en el micro 

#!echo "const unsigned char model_machine[] = {" > model_machine.h
#!cat gesture_model.tflite | xxd -i      >> model_machine.h
#!echo "};"                              >> model_machine.h

model_h_size = os.path.getsize("model.h")
print(f"Header file, model.h, is {model_h_size:,} bytes.")
print("\nOpen the side panel (refresh if needed). Double click model.h to download the file.")