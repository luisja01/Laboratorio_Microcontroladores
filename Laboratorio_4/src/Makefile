OBJS = sdram.o lcd.o clock.o
BINARY = labo4
LDSCRIPT = ../stm32f429i-discovery.ld

Upload:
	st-flash --reset write labo4.bin 0x8000000

include ../../Makefile.include

