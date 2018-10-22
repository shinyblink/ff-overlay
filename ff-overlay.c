// ┳━┓┳━┓  ┏━┓┓ ┳┳━┓┳━┓┳  ┳━┓┓ ┳
// ┣━ ┣━ ━━┃ ┃┃┏┛┣━ ┃┳┛┃  ┃━┫┗┏┛
// ┇  ┇    ┛━┛┗┛ ┻━┛┇┗┛┇━┛┛ ┇ ┇
// ff-overlay: overlay one farbfeld image with another
// Usage: <farbfeld source> | ff-overlay overlay.ff | <farbfeld sink>
// made by vifino. ISC (C) vifino 2018

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "conversion.h"

#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// IO helpers.
static inline void chew(FILE* file, void* buffer, size_t bytes) {
	if (!fread(buffer, bytes, 1, file)) {
		eprintf("wanted more bytes, didn't get any?\n");
		exit(1);
	}
}
static inline void spew(FILE* file, void* buffer, size_t bytes) {
	if (file)
		if (!fwrite(buffer, bytes, 1, file)) {
			eprintf("write failed.\n");
			exit(1);
		}
}

static void ffparse(FILE* food, FILE* out, uint32_t* w, uint32_t* h) {
	char buf[8];
	chew(food, buf, 8);
	if (strncmp(buf, "farbfeld", 8) != 0) {
		eprintf("file is not a farbfeld image?\n");
		exit(1);
	}
	spew(out, buf, 8);

	chew(food, buf, 8);
	*w = ntohl(*(uint32_t*)buf);
	*h = ntohl(*(uint32_t*)(buf + 4));
	if (!w || !h) {
		eprintf("image has zero dimension?\n");
		exit(1);
	}
	spew(out, buf, 8);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		eprintf("Usage: <farbfeld source> | %s overlay.ff | <farbfeld sink>\n", argv[0]);
		return 1;
	}

	// parse overlay.
	FILE* overlay = fopen(argv[1], "r");
	if (!overlay)
		err(2, "Failed to open file %s\n", argv[1]);
	uint32_t ow, oh;
	ffparse(overlay, NULL, &ow, &oh);

	// parse input image
	uint32_t w, h;
	ffparse(stdin, stdout, &w, &h);

	// MATH.
	unsigned int sw = (w >= ow) ? ((w / 2) - (ow / 2) + (ow % 1)) : 0;
	unsigned int sh = (h >= oh) ? ((h / 2) - (oh / 2) + (oh % 1)) : 0;
	unsigned int ew = MIN(w, sw + ow);
	unsigned int eh = MIN(h, sh + oh);

	// dinner time.
	uint16_t buf[4];
  uint16_t obuf[4];
	uint32_t x, y;
	FP bs[4];
	FP os[4];
	uint16_t oa, ba, a;
	uint16_t or, og, ob;
	uint16_t br, bg, bb;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			// nom.
			chew(stdin, buf, 8);
			if (sh <= y && eh > y && sw <= x && ew > x) {
				chew(overlay, obuf, 8);
				if (obuf[3]) { // not 0 behaves the same in BE as in LE.
#ifdef DOCONVERT
					qbeush2ush(buf, buf);
					qbeush2ush(obuf, obuf);
#endif
					qush2fp(buf, bs);
					qush2fp(obuf, os);
					srgb2rgb(bs, bs);
					srgb2rgb(os, os);

					// blend alpha
					oa = os[3];
					ba = bs[3];
					a = oa + ba * (1 - oa);

					// blend colors.
					os[0] = (os[0]*oa + bs[0]*ba * (1 - oa)) / a;
					os[1] = (os[1]*oa + bs[1]*ba * (1 - oa)) / a;
					os[2] = (os[2]*oa + bs[2]*ba * (1 - oa)) / a;

					rgb2srgb(os, os);
					qfp2ush(os, obuf);
#ifdef DOCONVERT
					qush2beush(obuf, obuf);
#endif
					spew(stdout, obuf, 8);
					continue;
				}
			}
			spew(stdout, buf, 8);
		}
		fflush(stdout);
	}
	return 0;
}
