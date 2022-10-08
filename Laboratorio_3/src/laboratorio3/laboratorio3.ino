/*
Laboratorio de Microcontrolares
Pollotron3000
*/

//Librerías utilizadas para manejo de pantalla LCD PCD854
#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
#include <PID_v1.h>

#define HUMEDAD A3 //Pin analógico para la humedad
#define CALENTADOR A5 //Pin analógico para circuito calentador
#define TEMPERATURA A2 //Pin analógico para circuito calentador
#define POT A4 //Pin analógico para circuito calentador

// Pines de la pantalla conectados al arduino (CLK-> 2| DIN-> 3| D/C-> 4| CS-> 5| RST-> 6)
Adafruit_PCD8544 display = Adafruit_PCD8544(2, 3, 4, 5, 6);

//Se definen todas las variables a utilizar
double humedad;
double error;
double mapeo_humedad;
double temp_sensada;
double mapeo_temp;
double temp_op;
double mapeo_op;
double salida_PID; 
double mapeo_salida; 

//Definimos nuestro controlador PID utilizando la libreria PID_v1
PID Pollometro(&error, &salida_PID, &mapeo_op, 2, 0, 0, DIRECT);
void setup() {

  Serial.begin(9600); // Habilita los puertos de comunicación 
  
  // Configuración de pines para los LEDs
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  // Se activa el display
  display.begin();

  //Inicializamos el controlador PID
  // Sensor humedad
  humedad = analogRead(HUMEDAD);
  mapeo_humedad = map(humedad, 0.0, 1023.0, 0.0, 100.0);
  //  Sensor temperatura
  temp_sensada = analogRead(TEMPERATURA);
  mapeo_temp = map(temp_sensada, 52.0, 759.0, 125.0, 0.0);
  //Potenciometro de operacion
  temp_op = analogRead(POT);
  mapeo_op = map(temp_op, 0.0, 1012.0, 30.0, 42.0);
  error = mapeo_op - mapeo_temp;
  Pollometro .SetMode(AUTOMATIC);

}

void loop() {

  // Sensor humedad
  humedad = analogRead(HUMEDAD);
  mapeo_humedad = map(humedad, 0.0, 1023.0, 0.0, 100.0);

  //  Sensor temperatura
  temp_sensada = analogRead(TEMPERATURA);
  mapeo_temp = map(temp_sensada, 52.0, 759.0, 125.0, 0.0);

  //Potenciometro de operacion
  temp_op = analogRead(POT);
  mapeo_op = map(temp_op, 0.0, 1012.0, 30.0, 42.0);
  error = mapeo_op - mapeo_temp;
  delay(100);

  //Aplica el control
  Pollometro.Compute();  
  mapeo_salida = map(salida_PID, 0.0, 255.0, 0.0, 80.0);
  analogWrite (CALENTADOR, salida_PID) ;
  Serial.print(error);
  //Encender LEDs
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);

  // Código para imprimir información en pantalla LCD
  display.clearDisplay(); 
  display.setCursor(0,0);
  display.print("Datos medidos"); 
  display.print("\n"); 
  display.print("--------------"); 
  display.print("\n"); 
  display.print("Humedad(%):"); // Se imprime la humedad medida
  display.println((int)mapeo_humedad);
  display.print("Control:"); // Se imprime la señal de control 
  display.println((int)mapeo_salida);
  display.print("Temp_s:"); // Se imprime la temperatura medida
  display.println((int)mapeo_temp);
  display.print("Temp_op:"); // Se imprime la temperatura de operación elegida
  display.println((int)mapeo_op);
  display.display();
}
