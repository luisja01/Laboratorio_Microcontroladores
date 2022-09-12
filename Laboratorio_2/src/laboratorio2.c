#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
  //Se configuran los registros
  DDRB = 0b01111111; //Configuracion del puerto
  DDRD = 0b11110011; //Entradas GPIOS 2 y 3

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
   if ((PD2 == 1)|(PD3 == 1))
   {
    PORTB = 0b01111111; 
   }
   else PORTB = 0b00000000;
   
  }
    return 0;
}
