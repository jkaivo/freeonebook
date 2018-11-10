#ifndef GPIO_H
#define GPIO_H

int gpio_get(int port);
void gpio_set(int port);
void gpio_clear(int port);
void gpio_init(void);

#endif
