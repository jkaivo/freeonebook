CC=arm-linux-gnueabihf-gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lpthread

freeonebook: freeonebook.o gpio.o

freeonebook.o: freeonebook.c gpio.h

gpio.o: gpio.c gpio.h

clean:
	rm -f freeonebook *.o
