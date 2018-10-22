// Small library for color conversion.
// All with alpha channel.
// Uses doubles for conversion.
// Not fast at all, but simple.
#include <stdint.h>
#include <math.h>

// I know, I know, not standardized.
// But painless fallback.
#if defined(__linux__)
#include <endian.h>
#if __BYTE_ORDER != __BIG_ENDIAN
#define DOCONVERT
#endif
#else
#define DOCONVERT
#include <arpa/inet.h>
#define be32toh ntohl
#define htobe32 htonl
#define be16toh ntohs
#define htobe16 htons
#endif

#ifndef FP
#define FP double
#endif

#if FP == float
#define POW powf
#else
#define POW pow
#endif

// BE uint16_t <-> NE uint16_t
static inline void qbeush2ush(uint16_t* src, uint16_t* dst) {
	dst[0] = be16toh(src[0]);
	dst[1] = be16toh(src[1]);
	dst[2] = be16toh(src[2]);
	dst[3] = be16toh(src[3]);
}
static inline void qush2beush(uint16_t* src, uint16_t* dst) {
	dst[0] = htobe16(src[0]);
	dst[1] = htobe16(src[1]);
	dst[2] = htobe16(src[2]);
	dst[3] = htobe16(src[3]);
}

// uint16 <-> FP 0..1
#define USH2FP(chan) (((FP) (chan)) / UINT16_MAX)
#define FP2USH(chan) ((uint16_t) (chan * UINT16_MAX))

static inline void qush2fp(uint16_t* src, FP* dst) {
	dst[0] = USH2FP(src[0]);
  dst[1] = USH2FP(src[1]);
	dst[2] = USH2FP(src[2]);
	dst[3] = USH2FP(src[3]);
}
static inline void qfp2ush(FP* src, uint16_t* dst) {
	dst[0] = FP2USH(src[0]);
  dst[1] = FP2USH(src[1]);
	dst[2] = FP2USH(src[2]);
	dst[3] = FP2USH(src[3]);
}

// SRGB <-> Linear RGB
#define SRGB_THRES ((FP) 0.0031306684425217108)
#define SRGB2RGB(s) ((s <= (SRGB_THRES * 12.92)) ? s / 12.92 : POW((s + 0.055) / 1.055, 2.4))
#define RGB2SRGB(l) ((l <= SRGB_THRES) ? l * 12.92 : 1.055 * POW(l, 1.0/2.4) - 0.055)

static inline void srgb2rgb(FP* src, FP* dst) {
	dst[0] = SRGB2RGB(src[0]);
  dst[1] = SRGB2RGB(src[1]);
	dst[2] = SRGB2RGB(src[2]);
	dst[3] = src[3]; // yep. very complicated.
}
static inline void rgb2srgb(FP* src, FP* dst) {
	dst[0] = RGB2SRGB(src[0]);
  dst[1] = RGB2SRGB(src[1]);
	dst[2] = RGB2SRGB(src[2]);
	dst[3] = src[3]; // yep. very complicated.
}

// Linear RGB <-> CIE XYZ
#define MAT3MUL(s, v1, v2, v3) (s[0] * (FP)(v1) + s[1] * (FP)(v2) + s[2] * (FP)(v3))

#define MAT3M3MUL(s, d, scale, r1m1, r1m2, r1m3, r2m1, r2m2, r2m3, r3m1, r3m2, r3m3) \
	(d)[0] = MAT3MUL((s), r1m1, r1m2, r1m3) * (FP)(scale);								\
	(d)[1] = MAT3MUL((s), r2m1, r2m2, r2m3) * (FP)(scale);								\
	(d)[2] = MAT3MUL((s), r3m1, r3m2, r3m3) * (FP)(scale);

static inline void rgb2xyz(FP* src, FP* dst) {
	// table stolen from blind.
	// seems to be a bit too bright?
	MAT3M3MUL(src, dst, 1,
						0.412457445582367576708548995157,
						0.357575865245515878143578447634,
						0.180437247826399665973085006954,
						0.212673370378408277403536885686,
						0.715151730491031756287156895269,
						0.072174899130559869164791564344,
						0.019333942761673460208893260415,
						0.119191955081838593666354597644,
						0.950302838552371742508739771438);

	dst[3] = src[3]; // yep. very complicated.
}

static inline void xyz2rgb(FP* src, FP* dst) {
	// table stolen from blind.
	MAT3M3MUL(src, dst, 1,
						3.240446254647737500675930277794,
						-1.537134761820080575134284117667,
						-0.498530193022728718155178739835,
						-0.969266606244679751469561779231,
						1.876011959788370209167851498933,
						0.041556042214430065351304932619,
						0.055643503564352832235773149705,
						-0.204026179735960239147729566866,
						1.057226567722703292062647051353);
	dst[3] = src[3];
}
