#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//Prueba de push
int main(void)
{

    DDRB = 0x07; //Configuracion del puerto

  //Parpadear
  while (1) {
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0x01; _delay_ms(5000);
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0x02; _delay_ms(5000);
    PORTB = 0x00; _delay_ms(5000);
    PORTB = 0x04; _delay_ms(5000);
  }

    return 0;
}
