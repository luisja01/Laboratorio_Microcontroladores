'''Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Laboratorio 3
Simulador de Incubadora de huevos
Ciclo: II-2022
'''
#Se importan las librerias
import serial 
import csv 
from time import sleep

fileName = "datos.csv" #Nombre del archivo que se generara

datos = serial.Serial("/tmp/ttyS1",9600,timeout=2) # Puerto a utilizar 

print("Connectado al puerto:" + "/tmp/ttyS1")

arch = open(fileName, "w") # Se crea el archivo csv 


encabezado = ['Humedad', 'Control', 'Temp_s', 'Temp_op']

# Se imprime encabezado 
print(encabezado[0]+','+encabezado[1]+','+encabezado[2]+','+encabezado[3])

arch.write(encabezado[0]+','+encabezado[1]+','+encabezado[2]+','+encabezado[3]+'\n')

# Loop para estar leyendo y escribiendo datos 
while True:
    sleep(0.18)

    if datos.in_waiting == 0:
        arch.close()
        print("Archivo creado")
        exit()

    getData=datos.readline() # Lectura de la linea
    dataString = getData.decode('utf-8') # Decdificación en formato utf-8
    data = dataString[0:][:-2]   
    print(data)

    arch = open(fileName, "a")
    arch.write(data + '\n')
