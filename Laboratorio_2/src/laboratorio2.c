#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
  //Se configuran los registros
  DDRB = 0b01111111; //Configuracion del puerto

  //Parpadear
  while (1) {
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0b01111111; _delay_ms(5000);
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0b01111111; _delay_ms(5000);
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0b01111111; _delay_ms(5000);
  }
    return 0;
}
