FAMILY=-mpic14
MCU=-p12f675
CC=sdcc
LD=sdcc
CFLAGS=-I. -I/usr/share/sdcc/include
TARGET=laboratorio1

SRCS = laboratorio1.c

all:
		${CC} --use-non-free ${FAMILY} ${MCU} ${CFLAGS} -o ${TARGET} ${SRCS}
#       ${LD} -o ${TARGET}.elf ${TARGET}.o                                      
#       ${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.o ${TARGET}.hex          
#       ${SIZE} -C --mcu=${MCU} ${TARGET}.elf                                   

clean:
		rm -f *.c~ *.h~ *.o *.elf *.hex *.asm                                
