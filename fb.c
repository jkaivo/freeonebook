#include <fcntl.h>
#include <stdio.h>
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

static char *fbaddr;

void dumpsi(const char *ctl, int io, struct fb_var_screeninfo *si)
{
	printf("- %s\n", ctl);
	printf("\tioctl returned %d\n", io);
	printf("\txres: %d\n", si->xres);
	printf("\tyres: %d\n", si->yres);
	printf("\txres_virtual: %d\n", si->xres_virtual);
	printf("\tyres_virtual: %d\n", si->yres_virtual);
	printf("\txoffset: %d\n", si->xoffset);
	printf("\tyoffset: %d\n", si->yoffset);
	printf("\tbits_per_pixel: %d\n", si->bits_per_pixel);
	printf("\tgrayscale: %d\n", si->grayscale);
}

void dumpfsi(const char *ctl, int io, struct fb_fix_screeninfo *fsi)
{
	printf("- %s\n", ctl);
	printf("\tioctl returned %d\n", io);
	printf("\tid: %16s\n", fsi->id);
	printf("\tsmem_len: %d\n", fsi->smem_len);
	printf("\ttype: %d\n", fsi->type);
	printf("\ttype_aux: %d\n", fsi->type_aux);
	printf("\tvisual: %d\n", fsi->visual);
	printf("\txpanstep: %d\n", fsi->xpanstep);
	printf("\typanstep: %d\n", fsi->ypanstep);
	printf("\tywrapstep: %d\n", fsi->ywrapstep);
	printf("\tline_length: %d\n", fsi->line_length);
	printf("\tmmio_start: %lu\n", fsi->mmio_start);
	printf("\tmmio_len: %d\n", fsi->mmio_len);
	printf("\taccel: %d\n", fsi->accel);
	printf("\tcapabilities: %d\n", fsi->capabilities);
}

void fb_init(void)
{
	printf("enabling left display\n");
	gpio_set(ENABLE_LEFT_DISPLAY);
	printf("enabling right display\n");
	gpio_set(ENABLE_RIGHT_DISPLAY);


	printf("mapping framebuffer...\n");
	struct fb_var_screeninfo si = { 0 };
	
	int fbfd = open("/dev/fb0", O_RDWR);

	struct fb_fix_screeninfo fsi;
	int io = ioctl(fbfd, FBIOGET_FSCREENINFO, &fsi);
	dumpfsi("FBIOGET_FSCREENINFO", io, &fsi);

	io = ioctl(fbfd, FBIOGET_VSCREENINFO, &si);
	dumpsi("FBIOEGET_VSCREENINFO", io, &si);

	si.bits_per_pixel = 8;
	si.grayscale = 1;
	si.activate = FB_ACTIVATE_FORCE;

	io = ioctl(fbfd, FBIOPUT_VSCREENINFO, &si); 
	dumpsi("FBIOPUT_VSCREENINFO", io, &si);

	fbaddr = mmap(NULL, fsi.smem_len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fbfd, 0);

	for (unsigned int i = 0; i < fsi.smem_len; i++) {
		fbaddr[i] = i % 2 == 0 ? '\0' : '\xff';
	}

	close(fbfd);
	printf("at %p\n", fbaddr);
}
