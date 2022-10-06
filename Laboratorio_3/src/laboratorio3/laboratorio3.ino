#define HUMEDAD A3


//Librerías utilizadas para manejo de pantalla LCD PCD854
#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>


// Pines de la pantalla conectados al arduino (CLK-> 2| DIN-> 3| D/C-> 4| CS-> 5| RST-> 6)
Adafruit_PCD8544 display = Adafruit_PCD8544(2, 3, 4, 5, 6);


void setup() {

  Serial.begin(9600); // Habilita los puertos de comunicación 
  
  // Configuración de pines para los LEDs
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  // Se activa el display
  display.begin();

}

void loop() {

  // Sensor humedad
  int humidity = analogRead(HUMEDAD);
  int mapeo_1 = map(humidity, 0, 1023, 0, 100);
  Serial.println(mapeo_1);
  delay(400);
 


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
  display.println(mapeo_1);
  display.print("Control:"); // Se imprime la señal de control 
  display.println(10);
  display.print("Temp_s:"); // Se imprime la temperatura medida
  display.println(15);
  display.print("Temp_op:"); // Se imprime la temperatura de operación elegida
  display.println(8);
  display.display();

}
