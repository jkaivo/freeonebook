# freeonebook

## Compiling

In order to compile this, you will need an ARM cross compiler (unless you are
building natively on ARM, in which case skip this step). You'll also need make
and pkg-config (otherwise ImageMagick won't build). On Debian, this is easy
with:

    $ sudo apt install gcc-arm-linux-gnueabihf make pkg-config

### Dependencies

Image support is provided by ImageMagick, which in turn depends on libpng,
which in turn depends on zlib. The eOneBook already has dynamic libraries for
zlib and libpng installed, but you still need them installed in your
cross-compilation environment so ImageMagick knows they will be there.

#### zlib

    $ wget http://www.zlib.net/zlib-1.2.11.tar.gz
    $ tar xvzf zlib-1.2.11.tar.gz
    $ cd zlib-1.2.11
    $ CHOST=arm-linux-gnueabihf ./configure --prefix=/usr/arm-linux-gnueabihf
    $ make
    $ sudo make install

#### libpng

    $ wget https://download.sourceforge.net/libpng/libpng-1.6.35.tar.gz
    $ tar xvzf libpng-1.6.35.tar.gz
    $ cd libpng-1.6.35
    $ ./configure --prefix=/usr/arm-linux-gnueabihf --host=arm-linux-gnueabihf
    $ make
    $ sudo make install

#### ImageMagick

ImageMagick is not installed on the eOneBook, so we'll configure it to build
as a static library.

    $ wget https://imagemagick.org/download/ImageMagick.tar.gz
    $ tar xvzf ImageMagick.tar.gz
    $ cd ImageMagick-7.0.8-14
    $ PKG_CONFIG_PATH=/usr/arm-linux-gnueabihf/lib/pkgconfig ./configure --host=arm-linux-gnueabihf --without-utilities --disable-openmp --disable-shared --prefix=/usr/arm-linux-gnueabihf
    $ make
    $ sudo make install

### freeonebook

Once dependencies are installed, simply run `make` in the freeonebook directory.

    $ make

## Installing

Copy `freeonebook` to `.a001` on your SD card:

    $ cp freeonebook /path/to/sd/.a001

## Using

Insert your SD card into your eOneBook and open it up. The program will run
automatically.

At present, the program is in pre-alpha state and simply initializes the
display, logs button presses to `stdout`, and cleanly shuts down when requested
by closing the eOneBook. Don't expect much.
