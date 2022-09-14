//Se incluten las librerias necesarias 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

//Estados a utilizar
#define setup 0
#define peatonal 1
#define vehicular 2 

//Declaración de función
void maquina_estados();

//Variables a utilizar;

int estado_actual;
int estado_siguiente;

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
  sei(); //Se habilitan las interrupciones globales

  //Las entradas son PD2 y PD3, donde sus interrupciones son PCINT13 y PCINT14 respectivamente, o INT0 e INT1
  //En este caso, las subrutinas de interrupciones cambiarán únicamente un cambio de estado. 
  //Loop que se ejecuta siempre
  while (1) {
    estado_actual = estado_siguiente;//Se actualiza el estado_actual
    maquina_estados(); //Se llama funcion de maquina de estados
    //Esto de repite indefinidamente
  }
    return 0;
}


//Función con implementación de máquina de estados
void maquina_estados(){
  
  switch(estado_actual){//Se utiliza un switch para considerar cada caso

    case setup:
      PORTB = 0b00000000;
      _delay_ms(10000);
      estado_siguiente = vehicular; 
      break;

    //Estado peatonal
    case peatonal:
      PORTB = 0b01111000;
      _delay_ms(10000);
      estado_siguiente = vehicular; 
      break;//Se sale de la funcion

    //Estado paso vehiculo
    case vehicular: 
      PORTB = 0b00000111;
      if (PIND & ((1 << PD2)|(1 << PD3)))//Si se estripa el botón cambiar de estado 
      { 
        estado_siguiente = peatonal; 
      }
      break; //Se sale de la funcion

  }

}
