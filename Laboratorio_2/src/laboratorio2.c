/*Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Laboratorio 2
Simulador de Semáforo
Ciclo: II-2022
*/

//Se incluten las librerias necesarias 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

//Estados a utilizar
#define setup 0
#define paso_peatonal 1
#define paso_vehicular 2 
#define parp 3

//Declaración de función
void maquina_estados();
void time_delay(int n);
void parpadear(int luces);
//Variables a utilizar;

int salida = 0x00;
int estado_actual;
int estado_siguiente;
bool cambio_estado = false;

//Funcion principal
int main(void)
{

  //Se inicializan los estados
  estado_siguiente = setup;

  //Se configuran los registros
  DDRB = 0b01111111; //Configuracion del puerto
  DDRD = 0b11110011; //Entradas GPIOS 2 y 3
  PIND = 0X00; //Se inicia en 0
  PORTD = 0x00; //Se inicia en 0
  PORTB = 0x00; //Se inicia en 0

  //Configuración para interrupciones con timer
  TCCR0A=0x00;   //Modo normnal del timer 
  TCCR0B=0x00;
  TCCR0B |= (1<<CS00)|(1<<CS02);   //Se pone el prescaler con un 1024
  TCNT0=0;    //Timer en 0
  sei(); //Se habilitan las interrupciones globales

  // Interrupciones 
  GIMSK |= (1<<INT0);  //  Habilitando interrupciones INT0 e INT1
  GIMSK |= (1<<INT1);
  //MCUCR |= (1<<ISC00)|(1<<ISC01); 


  //Las entradas son PD2 y PD3, donde sus interrupciones son PCINT13 y PCINT14 respectivamente, o INT0 e INT1
  //En este caso, las subrutinas de interrupciones cambiarán únicamente un cambio de estado. 
  //Loop que se ejecuta siempre
  while (1) {
    //estado_actual = estado_siguiente;//Se actualiza el estado_actual
    maquina_estados(); //Se llama funcion de maquina de estados
    //Esto de repite indefinidamente
  }
    return 0;
}


//Función con implementación de máquina de estados
void maquina_estados(){
  
  switch(estado_actual){//Se utiliza un switch para considerar cada caso

    case setup:
      salida = 0b00000000;
      PORTB = salida;
      estado_siguiente = paso_vehicular; 
      break;

    //Estado peatonal
    case paso_peatonal:
      salida = 0b00101001;
      PORTB = salida;
      time_delay(10000);
      estado_siguiente = paso_vehicular; 
      estado_actual = paso_vehicular;
      break;//Se sale de la funcion

    //Estado paso vehiculo
    case paso_vehicular: 
      salida = 0b01010100;
      PORTB = salida;
      estado_siguiente = parp;
      break; //Se sale de la funcion

    case parp:
      salida = 0b01010010;
      PORTB = salida;
      time_delay(6000);
      estado_siguiente = paso_peatonal;
      estado_actual = paso_peatonal;
      break; //Se sale de la funcion

  }

}

ISR (INT0_vect)        // Interrupt service routine 
{
  estado_actual=estado_siguiente;
}

ISR (INT1_vect)        // Interrupt service routine 
{
  estado_actual=estado_siguiente;
}


void time_delay(int n){
  unsigned int i=0; 
   while(i<=n)
   { 
      while((TIFR & (1 << TOV0) )==0);  //Se espera a que contador llegue de 0 a 255
      TIFR|=(1<<TOV0);                  // Se limpia la bandera al finalizar cuenta
      i++;                              // Se incrementa en 1
   }
}

void parpadear(int luces){
  PORTB = 0x00;
  time_delay(1000);
  PORTB = luces;
  time_delay(1000);
  PORTB = 0x00;
  time_delay(1000);
  PORTB = luces;
  time_delay(1000);
  PORTB = 0x00;
  time_delay(1000);
  PORTB = luces;
  time_delay(1000);
}
 