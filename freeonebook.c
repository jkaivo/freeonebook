#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO_EXPORT "/sys/class/gpio/export"

#define ENABLE_LEFT_PATH "/sys/class/gpio/gpio5/value"
#define ENABLE_RIGHT_PATH "/sys/class/gpio/gpio4/value"

#define ENABLE_VALUE "1"

void enable_displays(void)
{
	int left = open(ENABLE_LEFT_PATH, O_WRONLY);
	int right = open(ENABLE_RIGHT_PATH, O_WRONLY);
	
	printf("enabling left display\n");
	write(left, ENABLE_VALUE, sizeof(ENABLE_VALUE));
	printf("enabling right display\n");
	write(right, ENABLE_VALUE, sizeof(ENABLE_VALUE));

	close(left);
	close(right);
}

void gpio_init(void)
{
	struct {
		uint8_t port;
		enum { in, out } direction;
	} ports[] = {
		{ 100, in },	/* 4a58 */
		{ 91, in },
		{ 96, in },
		{ 89, in },
			{ 95, in },
			{ 92, in },
		{ 108, out },	/* 4aec */
		{ 94, out },
			{ 101, out },	/* 95, 92 */
		{ 5, out },
		{ 4, out },
		{ 93, in },
		{ 98, out },
		{ 90, out },
		{ 88, in },
		{ 0, out },
		{ 1, out },
		{ 2, out },
		{ 3, out },
		{ 6, out },
		{ 28, out },
		{ 29, out },
		{ 30, out },
		{ 33, out },
		{ 37, out },
		{ 38, out },
		{ 47, out },
		{ 48, out },
		{ 49, out },
		{ 50, out },
		{ 51, out },
		{ 52, out },
		{ 53, out },
		{ 54, out },
		{ 55, out },
		{ 56, out },
		{ 57, out },
		{ 58, out },
		{ 59, out },
		{ 60, out },
		{ 61, out },
		{ 62, out },
		{ 63, out },
		{ 64, out },
		{ 65, out },
		{ 66, out },
		{ 67, out },
		{ 68, out },
		{ 69, out },
		{ 70, out },
		{ 71, out },
		{ 72, out },
		{ 73, out },
		{ 74, out },
				{ 75, out },	/* not enabled */
		{ 78, out },
		{ 79, out },
		{ 86, out },
		{ 87, out },
		{ 97, out },
		{ 99, out },
		{ 104, out },
		{ 105, out },
		{ 106, out },
		{ 107, out },
		{ 109, out },
		{ 110, out },
		{ 111, out },
		{ 112, out },
		{ 113, out },
		{ 114, out },
		{ 115, out },
		{ 116, out },
		{ 117, out },
		{ 118, out },
		{ 119, out },
		{ 120, out },
		{ 121, out },
		{ 122, out },
		{ 135, out },
		{ 137, out },
		{ 138, out },
		{ 140, out },
		{ 144, out },
		{ 145, out },
		{ 146, out },
		{ 147, out },
		{ 148, out },
		{ 149, out },
	};
	int gpio_export = open(GPIO_EXPORT, O_WRONLY);
	for (int i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
		printf("enabling port %d\n", ports[i].port);
		dprintf(gpio_export, "%d", ports[i].port);
	}
	close(gpio_export);

	for (int i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
		char path[256];
		sprintf(path, "/sys/class/gpio/gpio%hhd/direction", ports[i].port);
		int dir = open(path, O_WRONLY);
		dprintf(dir, "%s", ports[i].direction == in ? "in" : "out");
		close(dir);
	}
}

int main(int argc, char *argv[])
{
	gpio_init();
	enable_displays();

	return 0;
}
