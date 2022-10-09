import serial #Libreria necesaria

port = "/tmp/ttyS1" #Puerto que se utiliza para la comunicacion
baud = 9600 #Velocidad del arduino
fileName = "datos.csv" #Nombre del archivo que se generara

ser = serial.Serial(port,baud)
print("Connectado al puerto:" + port)
file = open(fileName, "w")
print("Archivo creado")

getData=str(ser.readline())
data = getData[0:][:-2]
print(data)