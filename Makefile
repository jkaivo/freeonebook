CROSS=arm-linux-gnueabihf
DEPLIBS=MagickCore libpng16 zlib
PKGCONFIG=PKG_CONFIG_PATH=/usr/$(CROSS)/lib/pkgconfig pkg-config
CC=$(CROSS)-gcc
CFLAGS=-Wall -Wextra -I. $$($(PKGCONFIG) --cflags MagickCore)
LDFLAGS=-lpthread $$($(PKGCONFIG) --libs-only-L $(DEPLIBS))
LDLIBS=$$($(PKGCONFIG) --libs-only-l $(DEPLIBS)) -lm

all: install-deps freeonebook

freeonebook: freeonebook.o gpio.o fb.o convert.o

freeonebook.o: freeonebook.c gpio.h fb.h

gpio.o: gpio.c gpio.h

fb.o: fb.c fb.h convert.h

convert.o: convert.c convert.h

clean:
	rm -f freeonebook *.o

install-deps: deps.installed

deps.installed:
	cd deps; $(MAKE) all
	touch $@
