# freeonebook

## Compiling

In order to compile this, you will need an ARM cross compiler (unless you are
building natively on ARM, in which case skip this step). On Debian, this is
easy with:

    $ sudo apt install gcc-arm-linux-gnueabihf make

To compile, simply run `make`.

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
