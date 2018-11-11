#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "gpio.h"

#define FBSIZE (1186848 * 2)
#define SDPATH "/run/media/mmcblk0p1"

void enable_displays(void)
{
	printf("enabling displays\n");
	gpio_set(ENABLE_LEFT_DISPLAY);
	gpio_set(ENABLE_RIGHT_DISPLAY);
}

void halt(void)
{
	char *halt[] = { "shutdown", "-h", "now", NULL };
	execvp(halt[0], halt);
}

void poweroff(int reason)
{
	static int halting = 0;
	if (halting) {
		return;
	}
	halting = 1;

	printf("shutting down (%d)\n", reason);

	if (reason == GPIO_LOWBATTERY) {
		printf("low battery\n");
	}

	exit(0);
}

void buttonpress(int button)
{
	switch (button) {
	case BUTTON_SPECIAL:
		printf("special\n");
		break;

	case BUTTON_NEXTCHAPTER:
		printf("next chapter\n");
		break;

	case BUTTON_PREVBOOK:
		printf("previous book\n");
		break;

	case BUTTON_NEXTBOOK:
		printf("next book\n");
		break;

	case BUTTON_PREVPAGE:
		printf("previous page\n");
		break;

	case BUTTON_NEXTPAGE:
		printf("next page\n");
		break;

	default:
		printf("button %d pressed\n", button);
	}
}

int main(int argc, char *argv[])
{
	(void)argc; (void)argv;
	atexit(halt);

	gpio_init();
	gpio_watch(GPIO_LOWBATTERY, poweroff);
	gpio_watch(GPIO_SHUTDOWN, poweroff);
	gpio_watch(BUTTON_SPECIAL, buttonpress);
	gpio_watch(BUTTON_PREVBOOK, buttonpress);
	gpio_watch(BUTTON_PREVPAGE, buttonpress);
	gpio_watch(BUTTON_NEXTPAGE, buttonpress);
	gpio_watch(BUTTON_NEXTCHAPTER, buttonpress);
	gpio_watch(BUTTON_NEXTBOOK, buttonpress);

	enable_displays();
	
	for (;;) {
		sleep(INT_MAX);
	}

	return 0;
}
