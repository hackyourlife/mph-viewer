#ifndef __ENDIANESS_H__
#define __ENDIANESS_H__

#include "types.h"

static u16 get16bit_LE(u8* bytes) {
	return(bytes[0] | (bytes[1] << 8));
}

static u32 get32bit_LE(u8* bytes) {
	return(
		(bytes[0]) |
		(bytes[1] <<  8) |
		(bytes[2] << 16) |
		(bytes[3] << 24));
}

#endif
