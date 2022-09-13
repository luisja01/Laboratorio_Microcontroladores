#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>


//Estados a utilizar
#define peatonal 0
#define vehicular 1 

//Declaración de función
void maquina_estados();

//Variables a utilizar;
int estado_actual;
int estado_siguiente;

int main(void)
{

  //Se inicializan los estados

  estado_actual = peatonal;
  estado_siguiente = peatonal;

  //Se configuran los registros
  DDRB = 0b01111111; //Configuracion del puerto
  DDRD = 0b11110011; //Entradas GPIOS 2 y 3
  PORTD = 0x00;

  //Parpadear
  while (1) {
    /*
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0b01111111; _delay_ms(5000);
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0b01111111; _delay_ms(5000);
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0b01111111; _delay_ms(5000);
    */
    estado_actual = estado_siguiente;
    maquina_estados(); 
   
  }
    return 0;
}


//Función con implementación de máquina de estados
void maquina_estados(){
  
  switch(estado_actual){

    case peatonal:
      PORTB = 0b01111000;

      if (PIND & ((1 << PD2)|(1 << PD3)))//Si se estripa el botón cambiar de estado 
      { 
        estado_siguiente = vehicular; 
      }
      break;

    case vehicular: 
      PORTB = 0b00000111;
         if (PIND & (~(1 << PD2)|~(1 << PD3)))// Si se cambia estado de botón cambiar de estado 
        { 
        estado_siguiente = peatonal; 
        }
        break; 

  }

}
