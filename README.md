# freeonebook

In order to compile this, you will need an ARM cross compiler (unless you are
building natively on ARM, in which case skip this step). On Debian, this is
easy with:

    $ sudo apt install gcc-arm-linux-gnueabihf

To compile, simply run `make`.

    $ make

Then copy freeonebook to `.a001` on your SD card:

    $ cp freeonebook /path/to/sd/.a001

Insert your SD card into your eOneBook and open it up. The program will run
automatically.
