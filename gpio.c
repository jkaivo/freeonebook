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

static int gpio_open(int port, const char *field, int flags)
{
	char path[256];
	sprintf(path, "%s/gpio%d/%s", GPIO_BASEDIR, port, field);
	return open(path, flags);
}

int gpio_get(int port)
{
	char c;
	int fd = gpio_open(port, "value", O_RDONLY);
	read(fd, &c, 1);
	close(fd);

	return c != '0';
}

void gpio_set(int port)
{
	int fd = gpio_open(port, "value", O_WRONLY);
	write(fd, "1", 1);
	close(fd);
}

void gpio_clear(int port)
{
	int fd = gpio_open(port, "value", O_WRONLY);
	write(fd, "0", 1);
	close(fd);
}

static void gpio_enable_multi(int export_fd, int port, const char *state)
{
	dprintf(export_fd, "%d", port);
	int fd = gpio_open(port, "direction", O_WRONLY);
	write(fd, "direction", strlen(state));
	close(fd);
}

static void gpio_enable(int port, const char *state)
{
	int fd = open(GPIO_EXPORT, O_WRONLY);
	gpio_enable_multi(fd, port, state);
	close(fd);
}

void gpio_init(void)
{
	struct {
		uint8_t port;
		const char *initstate;
	} ports[] = {
		{ 108, GPIO_INIT_LOW },
		{ 94, GPIO_INIT_LOW },
		{ 101, GPIO_INIT_LOW },
		{ 5, GPIO_INIT_LOW },
		{ 4, GPIO_INIT_LOW },
		{ 98, GPIO_INIT_HIGH },
		{ 90, GPIO_INIT_HIGH },
		{ 0, GPIO_INIT_LOW },
		{ 1, GPIO_INIT_LOW },
		{ 2, GPIO_INIT_LOW },
		{ 3, GPIO_INIT_LOW },
		{ 6, GPIO_INIT_LOW },
		{ 28, GPIO_INIT_LOW },
		{ 29, GPIO_INIT_LOW },
		{ 30, GPIO_INIT_LOW },
		{ 33, GPIO_INIT_LOW },
		{ 37, GPIO_INIT_LOW },
		{ 38, GPIO_INIT_LOW },
		{ 47, GPIO_INIT_LOW },
		{ 48, GPIO_INIT_LOW },
		{ 49, GPIO_INIT_LOW },
		{ 50, GPIO_INIT_LOW },
		{ 51, GPIO_INIT_LOW },
		{ 52, GPIO_INIT_LOW },
		{ 53, GPIO_INIT_LOW },
		{ 54, GPIO_INIT_LOW },
		{ 55, GPIO_INIT_LOW },
		{ 56, GPIO_INIT_LOW },
		{ 57, GPIO_INIT_LOW },
		{ 58, GPIO_INIT_LOW },
		{ 59, GPIO_INIT_LOW },
		{ 60, GPIO_INIT_LOW },
		{ 61, GPIO_INIT_LOW },
		{ 62, GPIO_INIT_LOW },
		{ 63, GPIO_INIT_LOW },
		{ 64, GPIO_INIT_LOW },
		{ 65, GPIO_INIT_LOW },
		{ 66, GPIO_INIT_LOW },
		{ 67, GPIO_INIT_LOW },
		{ 68, GPIO_INIT_LOW },
		{ 69, GPIO_INIT_LOW },
		{ 70, GPIO_INIT_LOW },
		{ 71, GPIO_INIT_LOW },
		{ 72, GPIO_INIT_LOW },
		{ 73, GPIO_INIT_LOW },
		{ 74, GPIO_INIT_LOW },
		{ 75, GPIO_INIT_LOW },
		{ 78, GPIO_INIT_LOW },
		{ 79, GPIO_INIT_LOW },
		{ 86, GPIO_INIT_LOW },
		{ 87, GPIO_INIT_LOW },
		{ 97, GPIO_INIT_LOW },
		{ 99, GPIO_INIT_LOW },
		{ 104, GPIO_INIT_LOW },
		{ 105, GPIO_INIT_LOW },
		{ 106, GPIO_INIT_LOW },
		{ 107, GPIO_INIT_LOW },
		{ 109, GPIO_INIT_LOW },
		{ 110, GPIO_INIT_LOW },
		{ 111, GPIO_INIT_LOW },
		{ 112, GPIO_INIT_LOW },
		{ 113, GPIO_INIT_LOW },
		{ 114, GPIO_INIT_LOW },
		{ 115, GPIO_INIT_LOW },
		{ 116, GPIO_INIT_LOW },
		{ 117, GPIO_INIT_LOW },
		{ 118, GPIO_INIT_LOW },
		{ 119, GPIO_INIT_LOW },
		{ 120, GPIO_INIT_LOW },
		{ 121, GPIO_INIT_LOW },
		{ 122, GPIO_INIT_LOW },
		{ 135, GPIO_INIT_LOW },
		{ 137, GPIO_INIT_LOW },
		{ 138, GPIO_INIT_LOW },
		{ 140, GPIO_INIT_LOW },
		{ 144, GPIO_INIT_LOW },
		{ 145, GPIO_INIT_LOW },
		{ 146, GPIO_INIT_LOW },
		{ 147, GPIO_INIT_LOW },
		{ 148, GPIO_INIT_LOW },
		{ 149, GPIO_INIT_LOW },
	};

	int gpio_export = open(GPIO_EXPORT, O_WRONLY);
	for (size_t i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
		gpio_enable_multi(gpio_export, ports[i].port, ports[i].initstate);
	}
	close(gpio_export);
}

struct gpio_watcher {
	int port;
	void (*fn)(int);
	char lastvalue;
};

void *gpio_watch_thread(void *arg)
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
	printf("starting watcher for %d\n", port);
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
