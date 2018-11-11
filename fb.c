#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <linux/fb.h>
#include <sys/ioctl.h>

#include "fb.h"
#include "gpio.h"

struct imx_epdc_fb_mode {
	struct fb_videomode *vmode;
	int vscan_holdoff;
	int sdoed_width;
	int sdoed_delay;
	int sdoez_width;
	int sdoez_delay;
	int gdclk_hp_offs;
	int gdsp_offs;
	int gdoe_offs;
	int gdclk_offs;
	int num_ce;
};

static unsigned char *fbaddr;
static struct fb_fix_screeninfo fsi;
static struct fb_var_screeninfo vsi;

static void dumpvsi(const char *ctl, int io)
{
	printf("- %s\n", ctl);
	printf("\tioctl returned %d\n", io);
	printf("\txres: %d\n", vsi.xres);
	printf("\tyres: %d\n", vsi.yres);
	printf("\txres_virtual: %d\n", vsi.xres_virtual);
	printf("\tyres_virtual: %d\n", vsi.yres_virtual);
	printf("\txoffset: %d\n", vsi.xoffset);
	printf("\tyoffset: %d\n", vsi.yoffset);
	printf("\tbits_per_pixel: %d\n", vsi.bits_per_pixel);
	printf("\tgrayscale: %d\n", vsi.grayscale);
}

static void dumpfsi(const char *ctl, int io)
{
	printf("- %s\n", ctl);
	printf("\tioctl returned %d\n", io);
	printf("\tid: %16s\n", fsi.id);
	printf("\tsmem_len: %d\n", fsi.smem_len);
	printf("\ttype: %d\n", fsi.type);
	printf("\ttype_aux: %d\n", fsi.type_aux);
	printf("\tvisual: %d\n", fsi.visual);
	printf("\txpanstep: %d\n", fsi.xpanstep);
	printf("\typanstep: %d\n", fsi.ypanstep);
	printf("\tywrapstep: %d\n", fsi.ywrapstep);
	printf("\tline_length: %d\n", fsi.line_length);
	printf("\tmmio_start: %lu\n", fsi.mmio_start);
	printf("\tmmio_len: %d\n", fsi.mmio_len);
	printf("\taccel: %d\n", fsi.accel);
	printf("\tcapabilities: %d\n", fsi.capabilities);
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
	
	int fbfd = open("/dev/fb0", O_RDWR);

	int io = ioctl(fbfd, FBIOGET_FSCREENINFO, &fsi);
	dumpfsi("FBIOGET_FSCREENINFO", io);

	io = ioctl(fbfd, FBIOGET_VSCREENINFO, &vsi);
	dumpvsi("FBIOEGET_VSCREENINFO", io);

	vsi.bits_per_pixel = 8;
	vsi.grayscale = 1;
	vsi.activate = FB_ACTIVATE_FORCE;

	io = ioctl(fbfd, FBIOPUT_VSCREENINFO, &vsi); 
	dumpvsi("FBIOPUT_VSCREENINFO", io);

	fbaddr = mmap(NULL, fsi.smem_len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fbfd, 0);

	close(fbfd);
	printf("at %p\n", fbaddr);
}
