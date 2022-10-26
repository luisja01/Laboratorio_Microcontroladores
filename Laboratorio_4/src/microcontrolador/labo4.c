/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2014 Chuck McManis <cmcmanis@mcmanis.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <math.h>
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
/*
 * This is our example, the heavy lifing is actually in lcd-spi.c but
 * this drives that code.
 */


static void usart_setup(void)
{
	/* Setup USART2 parameters. */
	usart_set_baudrate(USART1, 9600);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);
}

int main(void)
{	
	char* bateria = "5";
	char* gyro_x  = "3";
	char* gyro_y  = "4";
	char* gyro_z  = "6";

	clock_setup();
	console_setup(115200);
	sdram_init();
	lcd_spi_init();
	usart_setup();
	//console_puts("LCD Initialized\n");
	//console_puts("Should have a checker pattern, press any key to proceed\n");
	//msleep(2000);
/*	(void) console_getc(1); */
	gfx_init(lcd_draw_pixel, 240, 320);
	

	while (1) {
		gfx_fillScreen(LCD_BLACK);
		//gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
		//gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
		//gfx_fillCircle(20, 250, 10, LCD_RED);
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
		usart_send_blocking(USART1, gyro_x);
		usart_send_blocking(USART1, ",");
		usart_send_blocking(USART1, gyro_y);
		usart_send_blocking(USART1, ",");
		usart_send_blocking(USART1, gyro_z);
		usart_send_blocking(USART1, ",");
		usart_send_blocking(USART1, bateria);
		usart_send_blocking(USART1, '\n');
	}
}
