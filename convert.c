#include <stdio.h>
#include <string.h>

#define MAGICKCORE_QUANTUM_DEPTH 8
#define MAGICKCORE_HDRI_ENABLE 0
#include <MagickCore/MagickCore.h>

int convert(const char *in, const char *out, int width, int height)
{
	if (!IsMagickCoreInstantiated()) {
		MagickCoreGenesis(NULL, MagickFalse);
	}

	printf("converting image %s\n", in);

	ExceptionInfo *exception = AcquireExceptionInfo();

	printf("reading original image\n");
	ImageInfo *info = CloneImageInfo((ImageInfo*)NULL);
	if (info == NULL) {
		printf("couldn't create new ImageInfo: ");
		CatchException(exception);
		return 1;
	}
	strcpy(info->filename, in);
	Image *original = ReadImage(info, exception);
	if (original == NULL) {
		printf("couldn't read original: ");
		CatchException(exception);
		return 0;
	}

	printf("resizing\n");
	Image *resized = ResizeImage(original, width, height, LanczosFilter, exception);
	DestroyImage(original);
	if (resized == NULL) {
		printf("couldn't resize: ");
		CatchException(exception);
		return 0;
	}

	printf("rotating\n");
	Image *rotated = IntegralRotateImage(resized, 3, exception);
	DestroyImage(resized);
	if (rotated == NULL) {
		printf("couldn't rotate: ");
		CatchException(exception);
		return 0;
	}

	printf("writing grayscale\n");
	strcpy(info->filename, out);
	if (!WriteImage(info, rotated, exception)) {
		printf("couldn't save converted image (%s): ", out);
		CatchException(exception);
		return 0;
	}

	printf("cleaning up\n");
	DestroyImage(rotated);
	DestroyImageInfo(info);
	DestroyExceptionInfo(exception);

	return 1;
}
