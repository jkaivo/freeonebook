CC=arm-linux-gnueabihf-gcc
CFLAGS=-Wall -Wextra

freeonebook: freeonebook.o

clean:
	rm -f freeonebook *.o
