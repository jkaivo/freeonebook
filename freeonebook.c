#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio.h"

#define BUTTON_SPECIAL		89
#define BUTTON_PREVBOOK		91
#define BUTTON_PREVPAGE		92
#define BUTTON_NEXTPAGE		95
#define BUTTON_NEXTCHAPTER	96
#define BUTTON_NEXTBOOK		100

#define ENABLE_LEFT_DISPLAY	5
#define ENABLE_RIGHT_DISPLAY	4

void enable_displays(void)
{
	gpio_set(ENABLE_LEFT_DISPLAY);
	gpio_set(ENABLE_RIGHT_DISPLAY);
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	gpio_init();
	enable_displays();

	return 0;
}
