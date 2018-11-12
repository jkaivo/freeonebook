CC=arm-linux-gnueabihf-gcc
CFLAGS=-Wall -Wextra -I. -IImageMagick
LDFLAGS=-lpthread -LImageMagick/MagickCore/.libs
LDLIBS=-lMagickCore-7.Q8 -lm

freeonebook: freeonebook.o gpio.o fb.o

freeonebook.o: freeonebook.c gpio.h fb.h

gpio.o: gpio.c gpio.h

fb.o: fb.c fb.h

clean:
	rm -f freeonebook *.o
