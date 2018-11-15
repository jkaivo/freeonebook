#include <stdio.h>
#include <string.h>

#include <MagickWand/MagickWand.h>

int convert(const char *in, const char *out, int width, int height)
{
	static MagickWand *w = NULL;
	static PixelWand *pixel = NULL;

	if (!IsMagickWandInstantiated()) {
		MagickWandGenesis();
		w = NewMagickWand();
		pixel = NewPixelWand();
	}

	if (!MagickReadImage(w, in)) {
		printf("couldn't read '%s'\n", in);
		return 1;
	}

	if (!MagickAdaptiveResizeImage(w, width, height)) {
		printf("couldn't resize image\n");
		return 1;
	}

	if (!MagickRotateImage(w, pixel, 270)) {
		printf("couldn't rotate image\n");
		return 1;
	}

	if (!MagickWriteImage(w, out)) {
		printf("couldn't write '%s'\n", out);
		return 1;
	}

	return 0;
}
