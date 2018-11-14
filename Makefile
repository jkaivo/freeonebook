CROSS=arm-linux-gnueabihf
DEPLIBS=MagickCore libpng16 zlib
PKG_CONFIG_PATH=./deps/lib/pkgconfig
CC=$(CROSS)-gcc
CFLAGS=-Wall -Wextra -I. $$(PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags MagickCore)
LDFLAGS=-lpthread $$(PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs-only-L $(DEPLIBS))
LDLIBS=$$(PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs-only-l $(DEPLIBS)) -lm

all: freeonebook

freeonebook: freeonebook.o gpio.o fb.o convert.o

freeonebook.o: freeonebook.c gpio.h fb.h

gpio.o: gpio.c gpio.h

fb.o: fb.c fb.h convert.h

convert.o: convert.c convert.h

clean:
	rm -f freeonebook *.o
