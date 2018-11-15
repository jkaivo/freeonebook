#include "gpio.h"

#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define GPIO_BASEDIR	"/sys/class/gpio"
#define GPIO_EXPORT	GPIO_BASEDIR "/export"

#define GPIO_IN		"in"
#define GPIO_INIT_LOW	"low"
#define GPIO_INIT_HIGH	"high"

struct gpio_watcher {
	int port;
	void (*fn)(int);
	char lastvalue;
};


static int gpio_open(int port, const char *field, int flags)
{
	char path[256];
	sprintf(path, "%s/gpio%d/%s", GPIO_BASEDIR, port, field);
	return open(path, flags);
}

static void gpio_enable(int port, const char *state)
{
	int export = open(GPIO_EXPORT, O_WRONLY);
	dprintf(export, "%d", port);
	close(export);

	int direction = gpio_open(port, "direction", O_WRONLY);
	write(direction, "direction", strlen(state));
	close(direction);
}

static void *gpio_watch_thread(void *arg)
{
	struct gpio_watcher *w = arg;
	int fd = gpio_open(w->port, "value", O_RDONLY);
	read(fd, &(w->lastvalue), 1);

	for (;;) {
		struct pollfd fds[] = { { fd, POLLPRI | POLLERR, 0 } };
		if ((poll(fds, 1, -1) == 1) && (fds[0].revents & POLLPRI)) {
			char value;
			lseek(fd, 0, SEEK_SET);
			read(fd, &value, 1);
			if (value != w->lastvalue) {
				w->fn(w->port);
				w->lastvalue = value;
			}
		}
	}

	close(fd);
	return NULL;
}

void gpio_watch(int port, void (*fn)(int))
{
	gpio_enable(port, "in");

	int edge = gpio_open(port, "edge", O_WRONLY);
	write(edge, "both", 4);
	close(edge);

	struct gpio_watcher *watcher = malloc(sizeof(*watcher));
	watcher->port = port;
	watcher->fn = fn;

	pthread_t thread;
	pthread_create(&thread, NULL, gpio_watch_thread, watcher);
}
