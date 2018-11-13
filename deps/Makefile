HOST=arm-linux-gnueabihf
PREFIX=/usr/$(HOST)

PKGCONFIG_PATH=/usr/$(HOST)/lib/pkgconfig

ZLIB_VERSION=1.2.11
LIBPNG16_VERSION=1.6.35
IMAGEMAGICK_VERSION=7.0.8-14

all: imagemagick.installed

zlib.installed:
	pkg-config --modversion zlib > $@ || $(MAKE) install-zlib

install-zlib:
	wget http://www.zlib.net/zlib-$(ZLIB_VERSION).tar.gz
	tar xvzf zlib-$(ZLIB_VERSION).tar.gz
	cd zlib-$(ZLIB_VERSION); CHOST=$(HOST) ./configure --static --prefix=$(PREFIX)
	cd zlib-$(ZLIB_VERSION); $(MAKE) && sudo $(MAKE) install
	pkg-config --modversion zlib > zlib.installed

libpng16.installed:
	pkg-config --modversion libpng16 > $@ || $(MAKE) install-libpng16

install-libpng16: zlib.installed
	wget https://download.sourceforge.net/libpng/libpng-$(LIBPNG16_VERSION).tar.gz
	tar xvzf libpng-$(LIBPNG16_VERSION).tar.gz
	cd libpng-$(LIBPNG16_VERSION); ./configure --prefix=$(PREFIX) --host=$(HOST)
	cd libpng-$(LIBPNG16_VERSION); $(MAKE) && sudo $(MAKE) install
	pkg-config --modversion libpng16 > libpng16.installed

imagemagick.installed:
	pkg-config --modversion MagickCore > $@ || $(MAKE) install-imagemagick

install-imagemagick: libpng16.installed
	wget https://imagemagick.org/download/ImageMagick.tar.gz
	tar xvzf ImageMagick.tar.gz
	cd ImageMagick-$(IMAGEMAGICK_VERSION); ./configure --host=$(HOST) --without-utilities --disable-shared --prefix=$(PREFIX) --disable-openmp --disable-docs
	cd ImageMagick-$(IMAGEMAGICK_VERSION); $(MAKE) && sudo $(MAKE) install
	

clean:
	rm -f *.installed
	rm -rf zlib-$(ZLIB_VERSION) libpng-$(LIBPNG16_VERSION) ImageMagic-$(IMAGEMAGICK_VERSION)
