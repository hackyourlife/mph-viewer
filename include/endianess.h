#ifndef __ENDIANESS_H__
#define __ENDIANESS_H__

#include "types.h"

static u16 get16bit_LE(u8* bytes) {
	return bytes[0] | (bytes[1] << 8);
}

static u32 get32bit_LE(u8* bytes) {
	return
		 bytes[0] |
		(bytes[1] <<  8) |
		(bytes[2] << 16) |
		(bytes[3] << 24);
}

static u32 get32bit_BE(u8* bytes) {
	return
		(bytes[0] << 24) |
		(bytes[1] << 16) |
		(bytes[2] <<  8) |
		 bytes[3];
}

#endif
