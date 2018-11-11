#ifndef GPIO_H
#define GPIO_H

#define ENABLE_RIGHT_DISPLAY    4
#define ENABLE_LEFT_DISPLAY     5

#define GPIO_LOWBATTERY         88
#define BUTTON_SPECIAL          89
#define BUTTON_PREVBOOK         91
#define BUTTON_PREVPAGE         92
#define GPIO_SHUTDOWN           93
#define BUTTON_NEXTPAGE         95
#define BUTTON_NEXTCHAPTER      96
#define BUTTON_NEXTBOOK         100

int gpio_get(int port);
void gpio_set(int port);
void gpio_clear(int port);
void gpio_init(void);
void gpio_watch(int port, void (*fn)(int));

#endif
