#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <linux/mxcfb.h>
#include <sys/ioctl.h>

#include "fb.h"
#include "gpio.h"

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
		.update_mode = UPDATE_MODE_PARTIAL,
		.waveform_mode = WAVEFORM_MODE_AUTO, /* DU? */
		.update_region.left = x,
		.update_region.top = y,
		.update_region.width = w,
		.update_region.height = h,
		.update_marker = marker,
		.temp = TEMP_USE_AMBIENT,
		.flags = 0,
	};
	ioctl(fb.fd, MXCFB_SEND_UPDATE, &data);

	struct mxcfb_update_marker_data md = {
		.update_marker = marker,
	};

	ioctl(fb.fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &md);
}

static void fb_blank(void)
{
	memset(fb.addr, 0xff, fb.size);
	fb_update(0, 0, fb.vsi.xres, fb.vsi.yres);
}

static void fb_loadimage(const char *path)
{
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		return;
	}

	char *img = mmap(NULL, fb.size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (img == MAP_FAILED) {
		return;
	}

	memcpy(fb.addr, img, fb.size);
	munmap(img, fb.size);
	close(fd);

	fb_update(0, 0, fb.vsi.xres, fb.vsi.yres);
}

void fb_init(void)
{
	/*
	printf("enabling left display\n");
	gpio_set(ENABLE_LEFT_DISPLAY);
	printf("enabling right display\n");
	gpio_set(ENABLE_RIGHT_DISPLAY);
	*/

	printf("mapping framebuffer...\n");
	
	fb.fd = open("/dev/fb0", O_RDWR);

	ioctl(fb.fd, FBIOGET_FSCREENINFO, &fb.fsi);

	ioctl(fb.fd, FBIOGET_VSCREENINFO, &fb.vsi);

	fb.vsi.bits_per_pixel = 8;
	fb.vsi.grayscale = GRAYSCALE_8BIT_INVERTED;
	fb.vsi.activate = FB_ACTIVATE_FORCE;

	ioctl(fb.fd, FBIOPUT_VSCREENINFO, &fb.vsi);

	fb.size = fb.vsi.xres_virtual * fb.vsi.yres_virtual;
	fb.addr = mmap(NULL, fb.size, PROT_READ | PROT_WRITE, MAP_SHARED, fb.fd, 0);

	printf("at %p\n", fb.addr);

	int updatemode = AUTO_UPDATE_MODE_REGION_MODE;
	ioctl(fb.fd, MXCFB_SET_AUTO_UPDATE_MODE, &updatemode);

	/*
	struct mxcfb_waveform_modes wave = {
		.mode_init = WAVEFORM_MODE_INIT,
		.mode_du = WAVEFORM_MODE_DU,
		.mode_gc4 = WAVEFORM_MODE_GC4,
		.mode_gc8 = WAVEFORM_MODE_GC16,
		.mode_gc16 = WAVEFORM_MODE_GC16,
		.mode_gc32 = WAVEFORM_MODE_GC16,
	};
	ioctl(fb.fd, MXCFB_SET_WAVEFORM_MODES, &wave);

	int scheme = UPDATE_SCHEME_QUEUE_AND_MERGE;
	ioctl(fb.fd, MXCFB_SET_UPDATE_SCHEME, &scheme);

	int powerdelay = 0;
	ioctl(fb.fd, MXCFB_SET_PWRDOWN_DELAY, &powerdelay);
	*/

	fb_blank();
	//fb_loadimage("/run/media/mmcblk0p1/left-screen.dat");
}
