#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "gpio.h"
#include "fb.h"

#define SDPATH "/run/media/mmcblk0p1"

#define LOWBATTERY_L "/home/root/SplashScreen/epdc_lowbatL.pgm"
#define LOWBATTERY_R "/home/root/SplashScreen/epdc_lowbatR.pgm"

static int fifo[2];

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
		fb_loadimage(RIGHT_SCREEN, LOWBATTERY_R);
		fb_loadimage(LEFT_SCREEN, LOWBATTERY_L);
	}

	exit(0);
}

void buttonpress(int button)
{
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&mutex);
	write(fifo[1], &button, sizeof(button));
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
	printf("starting freeonebook\n");
	(void)argc; (void)argv;
	printf("registering atexit() handler\n");
	atexit(halt);

	pipe(fifo);

	printf("adding watchers\n");
	gpio_watch(GPIO_LOWBATTERY, poweroff);
	gpio_watch(GPIO_SHUTDOWN, poweroff);
	gpio_watch(BUTTON_SPECIAL, buttonpress);
	gpio_watch(BUTTON_PREVBOOK, buttonpress);
	gpio_watch(BUTTON_PREVPAGE, buttonpress);
	gpio_watch(BUTTON_NEXTPAGE, buttonpress);
	gpio_watch(BUTTON_NEXTCHAPTER, buttonpress);
	gpio_watch(BUTTON_NEXTBOOK, buttonpress);

	printf("initializing framebuffer\n");
	fb_init();
	system("ls -lA "SDPATH"/grid.png");
	//fb_loadimage(RIGHT_SCREEN, SDPATH "/image.gray");
	fb_loadimage(RIGHT_SCREEN, SDPATH "/grid.png");
	//fb_loadimage(RIGHT_SCREEN, SDPATH "/right.gray");
	//fb_loadimage(LEFT_SCREEN, SDPATH "/left.gray");
	
	for (;;) {
		int button = 0;
		read(fifo[0], &button, sizeof(button));

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

		fflush(NULL);
		sync();
	}

	return 0;
}
