/*Universidad de Costa Rica
IE-0624 Laboratorio de Microcontroladores
Prof: Marco Villalta Fallas
Estudiantes: Juan Montealegre B95001 y Luis Herrera B93840
Laboratorio 4
Sismógrafo
Ciclo: II-2022
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include <errno.h>
#include "gfx.h"
#include "rcc.h"
#include "adc.h"
#include "dac.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"
#include <errno.h>
#include <unistd.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

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

#define L3GD20_SENSITIVITY_250DPS  (0.00875F)  
//Se crea struct para el gyroscopio
typedef struct Gyro {
  int16_t x;
  int16_t y;
  int16_t z;
} gyro;

int print_decimal(int);
gyro read_xyz(void);

static void spi_setup(void)
{
	rcc_periph_clock_enable(RCC_SPI5);
    rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOF);

    /* Se seleccina chip line */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);
    gpio_set(GPIOC, GPIO1);
    gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO7 | GPIO8 | GPIO9);   
	gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO8 | GPIO9);
    //Inicializacion del spi
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
	//Configuracion necesaria para el giroscopio tomada del ejemplo de la carpeta del f3
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_CTRL_REG1); 
	spi_read(SPI5);
	spi_send(SPI5, GYR_CTRL_REG1_PD | GYR_CTRL_REG1_XEN |
			GYR_CTRL_REG1_YEN | GYR_CTRL_REG1_ZEN |
			(3 << GYR_CTRL_REG1_BW_SHIFT));
	spi_read(SPI5);
	gpio_set(GPIOC, GPIO1); 
    gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_CTRL_REG4);
	spi_read(SPI5);
	spi_send(SPI5, (1 << GYR_CTRL_REG4_FS_SHIFT));
	spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);
}

static void usart_setup(void)
{
	/* Setup GPIO pins for USART1 transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	/* Setup USART1 TX pin as alternate function. */
	
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9);

	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	/* Finally enable the USART. */
	usart_enable(USART1);
}



gyro read_xyz(void)
{
	gyro lectura;
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_WHO_AM_I | 0x80);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_STATUS_REG | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_TEMP | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_X_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	lectura.x = spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_X_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	lectura.x |=spi_read(SPI5) << 8;
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Y_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	lectura.y =spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Y_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	lectura.y|=spi_read(SPI5) << 8;
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Z_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	lectura.z=spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Z_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	lectura.z|=spi_read(SPI5) << 8;
	gpio_set(GPIOC, GPIO1);

	lectura.x = lectura.x*L3GD20_SENSITIVITY_250DPS;
    lectura.y = lectura.y*L3GD20_SENSITIVITY_250DPS;
    lectura.z = lectura.z*L3GD20_SENSITIVITY_250DPS;
	return lectura;
}

static void gpio_setup(void)
{
	//Se habilitan los relojes
	rcc_periph_clock_enable(RCC_GPIOG);

	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
	/* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
			/* Set GPIO14 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
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

static void adc_setup(void)
{
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);
	adc_power_on(ADC1);

}

static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}


int main(void)
{	

	gyro lectura;
	float bateria_lvl;
	char data[10];
	char print_x[5];
	char print_y[5];
	char print_z[5];
	char print_bat[5];
	bool enviar = false; 
	uint16_t input_adc0;
	console_setup(115200);
	clock_setup();
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_ADC1);
	gpio_setup();
	adc_setup();
	sdram_init();
	usart_setup();
	spi_setup();
	lcd_spi_init();
	gfx_init(lcd_draw_pixel, 240, 320);

	while (1) {

		sprintf(print_x, "%s", "X:");
		sprintf(data, "%d", lectura.x);
		strcat(print_x, data);
		sprintf(print_y, "%s", "Y:");
		sprintf(data, "%d", lectura.y);
		strcat(print_y, data);
		sprintf(print_z, "%s", "Z:");
		sprintf(data, "%d", lectura.z);
		strcat(print_z, data);
		sprintf(print_bat, "%s", "V:");
		sprintf(data, "%f", bateria_lvl);
		strcat(print_bat, data);
		
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
		gfx_puts(print_x);
		gfx_setCursor(80, 94);
		gfx_puts(print_y);
		gfx_setCursor(145, 94);
		gfx_puts(print_z);
		gfx_setTextSize(2);
		gfx_setCursor(15, 120);
		gfx_puts("Bateria:");
		gfx_setCursor(15, 144);
		gfx_setTextSize(1);
		gfx_puts(print_bat);
		gfx_setCursor(114, 144);
		gfx_puts("V");
		gfx_setCursor(15, 270);
		gfx_setTextSize(1);
		gfx_puts("Estudiantes: Luis Herrera y");
		gfx_setCursor(15, 283);
		gfx_puts("Juan Montealegre");
		lcd_show_frame();
		gpio_clear(GPIOC, GPIO1); 
		//Enviar datos
		lectura = read_xyz();
		gpio_set(GPIOC, GPIO1); 
		input_adc0 = read_adc_naiive(1);
		//bateria_lvl = (input_adc0*5)/4095;
		bateria_lvl = (input_adc0*8.64)/62
		;
	
		if (enviar)
		{
			print_decimal(lectura.x);
			console_puts("\t");
       	 	print_decimal(lectura.y);
			console_puts("\t");
        	print_decimal(lectura.z); 
			console_puts("\t");
			print_decimal(bateria_lvl); 
			console_puts("\n");
			gpio_toggle(GPIOG, GPIO13);
		}

		else{
			gpio_clear(GPIOG, GPIO13);
		}

		if (bateria_lvl<7)
		{
			gpio_clear(GPIOG, GPIO14);
			gpio_toggle(GPIOG, GPIO14);
		}

		else gpio_clear(GPIOG, GPIO14);
		
		if (gpio_get(GPIOA, GPIO0)) {
			if (enviar) {
				enviar = false;
				gpio_clear(GPIOG, GPIO13);
				}
			else enviar = true;
		}

		int i;
		for (i = 0; i < 80000; i++)    /* Wait a bit. */
			__asm__("nop");
		
	}
	return 0;
}
