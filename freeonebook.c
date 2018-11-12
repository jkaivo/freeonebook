#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpio.h"
#include "fb.h"

#define FBSIZE (1186848 * 2)
#define SDPATH "/run/media/mmcblk0p1"

void halt(void)
{
	fflush(NULL);
	sync();

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
	} else {
		fb_blank();
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
		fb_loadimage(SDPATH "/right-screen.dat");
		break;

	case BUTTON_NEXTPAGE:
		printf("next page\n");
		fb_loadimage(SDPATH "/left-screen.dat");
		break;

	default:
		printf("button %d pressed\n", button);
	}

	fflush(NULL);
	sync();
}

int main(int argc, char *argv[])
{
	printf("starting freeonebook\n");
	(void)argc; (void)argv;
	printf("registering atexit() handler\n");
	atexit(halt);

	printf("adding watchers\n");
	//gpio_watch(GPIO_LOWBATTERY, poweroff);
	//gpio_watch(GPIO_SHUTDOWN, poweroff);
	gpio_watch(BUTTON_SPECIAL, buttonpress);
	gpio_watch(BUTTON_PREVBOOK, buttonpress);
	gpio_watch(BUTTON_PREVPAGE, buttonpress);
	gpio_watch(BUTTON_NEXTPAGE, buttonpress);
	gpio_watch(BUTTON_NEXTCHAPTER, buttonpress);
	gpio_watch(BUTTON_NEXTBOOK, buttonpress);

	printf("initializing framebuffer\n");
	fb_init();
	fb_blank();
	
	for (;;) {
		sleep(INT_MAX);
	}

	return 0;
}
