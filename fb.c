#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <linux/mxcfb.h>
#include <sys/ioctl.h>

#define MAGICKCORE_QUANTUM_DEPTH 8
#define MAGICKCORE_HDRI_ENABLE 0
#include <MagickCore/MagickCore.h>

#include "fb.h"
#include "gpio.h"

#define WAVEFORM_MODE_INIT      0x0     /* Screen goes to white (clears) */
#define WAVEFORM_MODE_DU        0x1     /* Grey->white/grey->black */
#define WAVEFORM_MODE_GC16      0x2     /* High fidelity (flashing) */
#define WAVEFORM_MODE_GC4       0x3     /* Lower fidelity */
#define WAVEFORM_MODE_A2        0x4     /* Fast black/white animation */

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
	int r = ioctl(fb.fd, MXCFB_SEND_UPDATE, &data);
	printf("%d\n", r); fflush(NULL); sync();

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
	printf("loading image %s\n", path);
	convert(path, "/tmp/image.gray", fb.vsi.xres, fb.vsi.yres);
	fflush(NULL); sync();

	int fd = open("/tmp/image.gray", O_RDONLY);
	if (fd == -1) {
		printf("not found\n");
		fflush(NULL);
		sync();
		return;
	}

	struct stat st;
	fstat(fd, &st);

	char *img = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (img == MAP_FAILED) {
		printf("map failed\n");
		fflush(NULL);
		sync();
		return;
	}

	printf("copying image..."); fflush(NULL); sync();
	unsigned char *buffer = fb.addr + (screen * fb.size / 2);
	memcpy(buffer, img, st.st_size);
	printf("done\n"); fflush(NULL); sync();

	munmap(img, st.st_size);
	close(fd);

	fb_update(0, (screen * fb.vsi.yres_virtual / 2), fb.vsi.xres_virtual, fb.vsi.yres_virtual / 2);
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

	fb_update(0, 0, fb.vsi.xres_virtual, fb.vsi.yres_virtual);

}
