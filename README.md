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
libpng installed, but you still need them installed in your cross-compilation
environment so ImageMagick knows they will be there. A make target is included
to simplify the process of installing them. If one or more is already installed
in your cross-compilation environment, it will be skipped.

    $ make install-deps

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
