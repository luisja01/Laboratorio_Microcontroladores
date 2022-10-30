/*Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Laboratorio 4
Sismógrafo
Ciclo: II-2022
*/

#include <stdint.h>
#include <math.h>
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include "rcc.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"
#include <errno.h>
#include <unistd.h>

//Se definen parametros necesarios para el giroscopio
#define GYR_RNW			(1 << 7) /* Write when zero */
#define GYR_MNS			(1 << 6) /* Multiple reads when 1 */
#define GYR_WHO_AM_I		0x0F
#define GYR_OUT_TEMP		0x26
#define GYR_STATUS_REG		0x27
#define GYR_CTRL_REG1		0x20
#define GYR_CTRL_REG1_PD	(1 << 3)
#define GYR_CTRL_REG1_XEN	(1 << 1)
#define GYR_CTRL_REG1_YEN	(1 << 0)
#define GYR_CTRL_REG1_ZEN	(1 << 2)
#define GYR_CTRL_REG1_BW_SHIFT	4
#define GYR_CTRL_REG4		0x23
#define GYR_CTRL_REG4_FS_SHIFT	4

#define GYR_OUT_X_L		0x28
#define GYR_OUT_X_H		0x29
#define GYR_OUT_Y_L		0x2A
#define GYR_OUT_Y_H		0x2B
#define GYR_OUT_Z_L		0x2C
#define GYR_OUT_Z_H		0x2D

int print_decimal(int);

static void spi_setup(void)
{
	//Configuracion spi para el giroscopio
	rcc_periph_clock_enable(RCC_SPI5);
	spi_set_master_mode(SPI5);
	spi_set_baudrate_prescaler(SPI5, SPI_CR1_BR_FPCLK_DIV_64);
	spi_set_clock_polarity_0(SPI5);
	spi_set_clock_phase_0(SPI5);
	spi_set_full_duplex_mode(SPI5);
	spi_set_unidirectional_mode(SPI5); /* bidirectional but in 3-wire */
	spi_enable_software_slave_management(SPI5);
	spi_send_msb_first(SPI5);
	spi_set_nss_high(SPI5);
	SPI_I2SCFGR(SPI5) &= ~SPI_I2SCFGR_I2SMOD;
	spi_enable(SPI5);
}

static void usart_setup(void)
{
	/* Setup USART2 parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	/* Finally enable the USART. */
	usart_enable(USART1);
}



static void read_xyz(int16_t vecs[3])
{
	uint8_t temp;
	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_WHO_AM_I | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	temp=spi_read(SPI5);
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_STATUS_REG | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	temp=spi_read(SPI5);
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_OUT_TEMP | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	temp=spi_read(SPI5);
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_OUT_X_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	vecs[0]=spi_read(SPI5);
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_OUT_X_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	vecs[0]|=spi_read(SPI5) << 8;
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_OUT_Y_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	vecs[1]=spi_read(SPI5);
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_OUT_Y_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	vecs[1]|=spi_read(SPI5) << 8;
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_OUT_Z_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	vecs[2]=spi_read(SPI5);
	gpio_set(GPIOE, GPIO3);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_OUT_Z_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	vecs[2]|=spi_read(SPI5) << 8;
	gpio_set(GPIOE, GPIO3);
}

static void gpio_setup(void)
{
	/* Enable GPIOG clock. */
	rcc_periph_clock_enable(RCC_GPIOG);

	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

	/* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, GPIO13);

			/* Set GPIO14 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, GPIO14);
}

int print_decimal(int num)
{
	/*
	int len = print_decimal(int value)
	Funcion para emprimir enteros en consola
	*/
	int		ndx = 0;
	char	buf[10];
	int		len = 0;
	char	is_signed = 0;

	if (num < 0) {
		is_signed++;
		num = 0 - num;
	}
	buf[ndx++] = '\000';
	do {
		buf[ndx++] = (num % 10) + '0';
		num = num / 10;
	} while (num != 0);
	ndx--;
	if (is_signed != 0) {
		console_putc('-');
		len++;
	}
	while (buf[ndx] != '\000') {
		console_putc(buf[ndx--]);
		len++;
	}
	return len; /* number of characters printed */
}

int main(void)
{	

	char bateria = "0";
	int16_t vecs[3];
	int bateria_lvl = 5;
	char gyro_x = "0";
	char gyro_y = "0";
	char gyro_z = "0";
	bool enviar = false; 
	uint32_t cr_tmp;
	clock_setup();
	gpio_setup();
	console_setup(115200);
	sdram_init();
	usart_setup();
	spi_setup();
	//Inicializacion necesario de acuerdo a ejemplo
	gpio_clear(GPIOE, GPIO1);
	spi_send(SPI5, GYR_CTRL_REG1);
	spi_read(SPI5);
	spi_send(SPI5, GYR_CTRL_REG1_PD | GYR_CTRL_REG1_XEN |
			GYR_CTRL_REG1_YEN | GYR_CTRL_REG1_ZEN |
			(3 << GYR_CTRL_REG1_BW_SHIFT));
	spi_read(SPI5);
	gpio_set(GPIOE, GPIO1);

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI5, GYR_CTRL_REG4);
	spi_read(SPI5);
	spi_send(SPI5, (1 << GYR_CTRL_REG4_FS_SHIFT));
	spi_read(SPI5);
	gpio_set(GPIOE, GPIO3);

	lcd_spi_init();
	gfx_init(lcd_draw_pixel, 240, 320);

	while (1) {
		sprintf(gyro_x, "%d", vecs[0]);
		sprintf(gyro_y, "%d", vecs[1]);
		sprintf(gyro_z, "%d", vecs[2]);
		sprintf(bateria, "%d", bateria_lvl);
	
		
		gfx_fillScreen(LCD_BLACK);
		//Desplagar en pantalla
		gfx_setTextColor(LCD_GREEN, LCD_BLACK);
		gfx_setTextSize(2);
		gfx_setCursor(15, 25);
		gfx_puts("Laboratorio 4");
		gfx_setTextSize(1);
		gfx_setCursor(15, 49);
		gfx_puts("Sismografo");
		gfx_setTextSize(2);
		gfx_setCursor(15, 70);
		gfx_puts("Orientacion:");
		gfx_setCursor(15, 94);
		gfx_setTextSize(1);
		gfx_puts("X:");
		gfx_setCursor(30, 94);
		gfx_puts(gyro_x);
		gfx_setCursor(50, 94);
		gfx_puts("Y:");
		gfx_setCursor(65, 94);
		gfx_puts(gyro_y);
		gfx_setCursor(85, 94);
		gfx_puts("Z:");
		gfx_setCursor(100, 94);
		gfx_puts(gyro_z);
		gfx_setTextSize(2);
		gfx_setCursor(15, 120);
		gfx_puts("Bateria:");
		gfx_setCursor(15, 144);
		gfx_setTextSize(1);
		gfx_puts(bateria);
		gfx_setCursor(30, 144);
		gfx_puts("V");
		gfx_setCursor(15, 270);
		gfx_setTextSize(1);
		gfx_puts("Estudiantes: Luis Herrera y");
		gfx_setCursor(15, 283);
		gfx_puts("Juan Montealegre");
		lcd_show_frame();
		
		//Enviar datos
		
		read_xyz(vecs);

		if (enviar)
		{
			print_decimal(vecs[0]); console_puts("\t");
       	 	print_decimal(vecs[1]); console_puts("\t");
        	print_decimal(vecs[2]); console_puts("\n");
			gpio_toggle(GPIOG, GPIO13);
		}

		else{
			gpio_clear(GPIOC, GPIO13);
		}

		if (gpio_get(GPIOA, GPIO0)) {
			if (enviar) {
				enviar = false;
				gpio_clear(GPIOC, GPIO13);
				}
			else enviar = true;
		}

		int i;
		for (i = 0; i < 80000; i++)    /* Wait a bit. */
			__asm__("nop");
		
	}
	return 0;
}
