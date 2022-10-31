CFLAGS = -DTEST
OBJS = sdram.o clock.o console.o lcd-spi.o gfx.o

BINARY = labo4

# we use sin/cos from the library
LDLIBS += -lm

LDSCRIPT = ../stm32f429i-discovery.ld

Upload:
	st-flash --reset write labo4.bin 0x8000000

include ../../Makefile.include
