#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdlib.h>

#include <linux/mxcfb.h>
#include <sys/ioctl.h>

#include "convert.h"
#include "fb.h"
#include "gpio.h"

#define CACHE_DIR	"/run/media/mmcblk0p1/.freeonebook"
#define LEFT_CACHE	CACHE_DIR "/left.gray"
#define RIGHT_CACHE	CACHE_DIR "/right.gray"

static struct {
	int fd;
	unsigned char *addr;
	struct fb_fix_screeninfo fsi;
	struct fb_var_screeninfo vsi;
	size_t size;
} fb;

static void fb_update(int x, int y, int w, int h)
{
	static int marker = 0;
	marker++;

	struct mxcfb_update_data data = {
		.update_mode = UPDATE_MODE_FULL,
		.waveform_mode = WAVEFORM_MODE_AUTO,
		.update_region.left = x,
		.update_region.top = y,
		.update_region.width = w,
		.update_region.height = h,
		.update_marker = marker,
		.temp = TEMP_USE_AMBIENT,
		.flags = 0,
	};

	printf("SEND_UPDATE: "); fflush(NULL); sync();
	int r = -1;
	int retries = 5;
	do {
		r = ioctl(fb.fd, MXCFB_SEND_UPDATE, &data);
		printf("%d;", r);
	} while (r == -1 && sleep(1) != INT_MAX && --retries > 0);
	printf("%d\n", r); fflush(NULL); sync();
	if (r == -1) {
		perror("SEND_UPDATE");
	}

	struct mxcfb_update_marker_data md = {
		.update_marker = marker,
	};
	printf("WAIT_FOR_UPDATE_COMPLETE: "); fflush(NULL); sync();
	r = ioctl(fb.fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &md);
	printf("%d\n", r); fflush(NULL); sync();
}

void fb_blank(void)
{
	memset(fb.addr, 0xff, fb.size);
	fb_update(0, 0, fb.vsi.xres_virtual, fb.vsi.yres_virtual);
}

void fb_loadimage(int screen, const char *path)
{
	const char *cache = screen == LEFT_SCREEN ? LEFT_CACHE : RIGHT_CACHE;
	struct stat st;
	stat(path, &st);

	if (st.st_size != fb.vsi.xres * fb.vsi.yres) {
		mkdir(CACHE_DIR, 0755);
		printf("converting image\n");
		convert(path, cache, fb.vsi.xres, fb.vsi.yres);
		path = cache;
	} else {
		/* copy to cache */
	}

	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror(path);
		return;
	}

	fstat(fd, &st);

	printf("mapping %zd bytes\n", st.st_size);
		fflush(NULL); sync();
	char *img = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (img == MAP_FAILED) {
		perror("mmap of image");
		return;
	}

	printf("copying image..."); fflush(NULL); sync();
	unsigned char *buffer = fb.addr + (screen * fb.size / 2);
	for (int y = 0; y < fb.vsi.yres; y++) {
		memcpy(buffer + (y * fb.vsi.xres_virtual), img + (y * fb.vsi.xres), fb.vsi.xres);
	}
	memcpy(buffer, img, st.st_size);
	printf("done\n"); fflush(NULL); sync();

	munmap(img, st.st_size);

	close(fd);

	fb_update(0, 0, fb.vsi.xres, fb.vsi.yres);
}

void fb_init(void)
{
	fb.fd = open("/dev/fb0", O_RDWR);

	ioctl(fb.fd, FBIOGET_FSCREENINFO, &fb.fsi);

	ioctl(fb.fd, FBIOGET_VSCREENINFO, &fb.vsi);

	fb.vsi.bits_per_pixel = 8;
	fb.vsi.grayscale = GRAYSCALE_8BIT;
	fb.vsi.activate = FB_ACTIVATE_FORCE;

	ioctl(fb.fd, FBIOPUT_VSCREENINFO, &fb.vsi);

	fb.size = fb.vsi.xres_virtual * fb.vsi.yres_virtual;
	fb.addr = mmap(NULL, fb.size, PROT_READ | PROT_WRITE, MAP_SHARED, fb.fd, 0);
}
