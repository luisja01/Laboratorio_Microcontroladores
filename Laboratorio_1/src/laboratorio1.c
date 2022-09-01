#include <pic14/pic12f675.h>
 
// Configuración de registro CONFIG
typedef unsigned int word;
word __at 0x2007 __CONFIG = (_WDTE_OFF & _WDT_OFF & _MCLRE_OFF);  // WDT y MCLR OFF


void delay (unsigned int tiempo);
void mostrar_num (int numero, int display); 
unsigned int rando(unsigned int min, unsigned int max);
 
void main(void)
{

    TRISIO = 0b00001000; //Poner el pin 3 como entrada y los demás como salidas
	GPIO = 0x00; //Poner pines en bajo
 
	unsigned int num1, num2;
	unsigned int cuenta_final = 0;
	
    //Loop para siempre 
    while ( 1 )
    {
		// Se generan los dos números aletorios
		num1 = rando(0,9);
		num2 = rando(0,9); 

		if (GP3 ==1)
		{
			cuenta_final=cuenta_final+1; // Si se estripa el botón se aumenta la cuenta 

			while (GP3 == 1)
			{
				// Cuando el botón se estrtipa se muestran los dos números random generados 
				mostrar_num(num1,0);
				delay(5);
				mostrar_num(num2,1);
				delay(5); 
			}	

			if (cuenta_final ==16)
			{
				// Si la cuenta llega a 16 entonces se alternan con el número
				// 9 en los displays
				mostrar_num(9,0);
				delay(200);
				mostrar_num(9,1);
				delay(200);
				mostrar_num(9,0);
				delay(200);
				mostrar_num(9,1);
				delay(200);
				mostrar_num(9,0);
				delay(200);
				mostrar_num(9,1);
				delay(200);
				cuenta_final = 0; // Se vuelve a poner la cuenta en 0 
			}
		}

		// Mientras no se precione el botón se están generando y mostrando
		// números aletorios en los displays generando el efecto de tómbola
		mostrar_num(num1,0);
		delay(5);
		mostrar_num(num2,1);
		delay(5);

	}
 
}

//Función para mostrar los números en cada display 
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


// Función para obtener número aleatorio
unsigned int rando(unsigned int min, unsigned int max)
  {
	static unsigned int rand = 0xAFEBU; 
    rand += 0x3FD;
    rand %= max;
    while(rand < min){
        rand = rand + max - min;
    }
    return rand;
  
  }

// Función de retraso 
void delay(unsigned int tiempo)
{
	unsigned int i;
	unsigned int j;

	for(i=0;i<tiempo;i++)
	  for(j=0;j<1275;j++);
}
