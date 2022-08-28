#include <pic14/pic12f675.h>
 
// Configuración de registro CONFIG
typedef unsigned int word;
word __at 0x2007 __CONFIG = (_WDTE_OFF & _WDT_OFF & _MCLRE_OFF);  // WDT y MCLR OFF


void delay (unsigned int tiempo);
void mostrar_num (int numero, int display); 
unsigned int rando(unsigned int start_range,unsigned int end_range);
 
void main(void)
{

    TRISIO = 0b00001000; //Poner el pin 3 como entrada y los demás como salidas
	GPIO = 0x00; //Poner pines en bajo
 
	//unsigned int cuenta; 
	unsigned int num1, num2;
	unsigned int cuenta = 0 ;
 
    //Loop forever
    while ( 1 )
    {
		
		num1 = rando(0,9);
		num2 = rando(0,9); 

		if (GP3 ==1)
		{
			cuenta =+cuenta+1;
		}

		if (cuenta ==17)
		{	
			cuenta = 0;  
			while (GP3==1)
				mostrar_num(9,0);
				delay(5);
				mostrar_num(9,1);
				delay(5);
		}

		while (GP3 == 1)
			{
			mostrar_num(num1,0);
			delay(5);
			mostrar_num(num2,1);
			delay(5); 
			}

		mostrar_num(num1,0);
		delay(5);
		mostrar_num(num2,1);
		delay(5);
   
	}
 
}

void mostrar_num (int numero, int display)
{


	if (display == 0)
	{
		if (numero == 0) GPIO = 0b00000000;

		else if (numero == 1) GPIO = 0b00000001; 

		else if (numero == 2) GPIO = 0b00000010; 

		else if (numero == 3) GPIO = 0b00000011;

		else if (numero == 4) GPIO = 0b00000100;

		else if (numero == 5) GPIO = 0b00000101;

		else if (numero == 6) GPIO = 0b00000110;

		else if (numero == 7) GPIO = 0b00000111;

		else if (numero == 8) GPIO = 0b00010000;

		else GPIO = 0b00010001;
	}

	else if (display == 1)
	{
		if (numero == 0) GPIO = 0b00100000;

		else if (numero == 1) GPIO = 0b00100001; 

		else if (numero == 2) GPIO = 0b00100010; 

		else if (numero == 3) GPIO = 0b00100011;

		else if (numero == 4) GPIO = 0b00100100;

		else if (numero == 5) GPIO = 0b00100101;

		else if (numero == 6) GPIO = 0b00100110;

		else if (numero == 7) GPIO = 0b00100111;

		else if (numero == 8) GPIO = 0b00110000;

		else GPIO = 0b00110001;
	}


}


unsigned int rando(unsigned int start_range,unsigned int end_range)
  {
    static unsigned int rand = 0xACE1U; /* Any nonzero start state will work. */

    /*check for valid range.*/
    if(start_range == end_range) {
        return start_range;
    }

    /*get the random in end-range.*/
    rand += 0x3AD;
    rand %= end_range;

    /*get the random in start-range.*/
    while(rand < start_range){
        rand = rand + end_range - start_range;
    }

    return rand;
  }

void delay(unsigned int tiempo)
{
	unsigned int i;
	unsigned int j;

	for(i=0;i<tiempo;i++)
	  for(j=0;j<1275;j++);
}