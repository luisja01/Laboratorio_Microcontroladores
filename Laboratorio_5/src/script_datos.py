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

fileName = "estacionario.csv" #Nombre del archivo que se generara

#datos = serial.Serial("/dev/ttyACM1",115200,timeout=2) # Puerto a utilizar 
datos = serial.Serial("/dev/ttyACM0",115200) # Puerto a utilizar 

print("Connectado al puerto:" + "/dev/ttyACM1")
#print("Connectado al puerto:" + "/dev/ttyACM0")

arch = open(fileName, "w", encoding='UTF8') # Se crea el archivo csv 

encabezado = ['Eje X', 'Eje Y', 'Eje Z']

# Se imprime encabezado 
print(encabezado[0]+','+encabezado[1]+','+encabezado[2])
arch.write(encabezado[0]+','+encabezado[1]+','+encabezado[2]+'\n')

cont = 0
# Loop para estar leyendo y escribiendo datos 
while True:
    data = datos.readline().decode('utf-8').replace('\r', "").replace('\n', "")
    data = data.split('\t')
    arch = open(fileName, "a")
    arch.write(data[0]+','+data[1]+','+data[2]+'\n')
    print(data)
    print(cont)
    cont += 1
    if cont ==2000:
        exit()

