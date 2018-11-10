#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO_BASEDIR	"/sys/class/gpio"
#define GPIO_EXPORT	GPIO_BASEDIR "/export"

#define ENABLE_LEFT_PATH	GPIO_BASEDIR "/gpio5/value"
#define ENABLE_RIGHT_PATH	GPIO_BASEDIR "/gpio4/value"

#define ENABLE_VALUE "1"

void gpio_write(int port, const char *field, const char *value)
{
	char path[256];
	sprintf(path, "%s/gpio%d/%s", GPIO_BASEDIR, field);
	int fd = open(path, O_WRONLY);
	write(fd, value, strlen(value));
	close(fd);
}

void enable_displays(void)
{
	gpio_write(5, "value", "1");
	gpio_write(4, "value", "1");
}

void gpio_init(void)
{
	struct {
		uint8_t port;
		enum { in, out } direction;
		uint8_t value;
	} ports[] = {
		{ 100, in },
		{ 91, in },
		{ 96, in },
		{ 89, in },
			{ 95, in },
			{ 92, in },
				{ 108, out, 0 },
				{ 94, out, 0 },
				{ 101, out, 0 },	/* 95, 92 */
		{ 5, out },
		{ 4, out },
		{ 93, in },
		{ 98, out, 1 },
				{ 90, out, 1 },			/* 108, 94, 101 */
		{ 88, in },
		{ 0, out, 0 },
		{ 1, out, 0 },
		{ 2, out, 0 },
		{ 3, out, 0 },
		{ 6, out, 0 },
		{ 28, out, 0 },
		{ 29, out, 0 },
		{ 30, out, 0 },
		{ 33, out, 0 },
		{ 37, out, 0 },
		{ 38, out, 0 },
		{ 47, out, 0 },
		{ 48, out, 0 },
		{ 49, out, 0 },
		{ 50, out, 0 },
		{ 51, out, 0 },
		{ 52, out, 0 },
		{ 53, out, 0 },
		{ 54, out, 0 },
		{ 55, out, 0 },
		{ 56, out, 0 },
		{ 57, out, 0 },
		{ 58, out, 0 },
		{ 59, out, 0 },
		{ 60, out, 0 },
		{ 61, out, 0 },
		{ 62, out, 0 },
		{ 63, out, 0 },
		{ 64, out, 0 },
		{ 65, out, 0 },
		{ 66, out, 0 },
		{ 67, out, 0 },
		{ 68, out, 0 },
		{ 69, out, 0 },
		{ 70, out, 0 },
		{ 71, out, 0 },
		{ 72, out, 0 },
		{ 73, out, 0 },
		{ 74, out, 0 },
				{ 75, out, 0 },	/* not enabled */
		{ 78, out, 0 },
		{ 79, out, 0 },
		{ 86, out, 0 },
		{ 87, out, 0 },
		{ 97, out, 0 },
		{ 99, out, 0 },
		{ 104, out, 0 },
		{ 105, out, 0 },
		{ 106, out, 0 },
		{ 107, out, 0 },
		{ 109, out, 0 },
		{ 110, out, 0 },
		{ 111, out, 0 },
		{ 112, out, 0 },
		{ 113, out, 0 },
		{ 114, out, 0 },
		{ 115, out, 0 },
		{ 116, out, 0 },
		{ 117, out, 0 },
		{ 118, out, 0 },
		{ 119, out, 0 },
		{ 120, out, 0 },
		{ 121, out, 0 },
		{ 122, out, 0 },
		{ 135, out, 0 },
		{ 137, out, 0 },
		{ 138, out, 0 },
		{ 140, out, 0 },
		{ 144, out, 0 },
		{ 145, out, 0 },
		{ 146, out, 0 },
		{ 147, out, 0 },
		{ 148, out, 0 },
		{ 149, out, 0 },
	};

	int gpio_export = open(GPIO_EXPORT, O_WRONLY);
	for (int i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
		printf("enabling port %d\n", ports[i].port);
		dprintf(gpio_export, "%d", ports[i].port);
	}
	close(gpio_export);

	for (int i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
		gpio_write(ports[i].port, "direction", ports[i].direction == in ? "in" : "out");
	}

	for (int i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
		if (ports[i].direction == out) {
			gpio_write(ports[i].port, "value", ports[i].value == 1 ? "1" : "0");
		}
	}
}

int main(int argc, char *argv[])
{
	gpio_init();
	enable_displays();

	return 0;
}
