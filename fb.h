#ifndef FB_H
#define FB_H

#define RIGHT_SCREEN	0
#define LEFT_SCREEN	1

void fb_init(void);
void fb_blank(void);
void fb_loadimage(int screen, const char *path);

#endif
