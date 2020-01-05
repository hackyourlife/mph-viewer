#include "types.h"
#include "error.h"
#include "heap.h"
#include "lzss.h"

#define N		 4096	/* size of ring buffer */
#define F		   18	/* upper limit for match_length */
#define THRESHOLD	    2   /* encode string into position and length if match_length is greater than this */

int LZDecode(void** output, void* input)
{
	unsigned char text_buf[N + F - 1];

	int  i, j, k, r, c, z;
	unsigned int flags;
	u32 decomp_size;
	u32 cur_size = 0;
	u8* in = (u8*) input;
	u8* out;

	// read gba header info
	u32 gbaheader;
	for(i = 0; i < 4; i++)
		gbaheader = (gbaheader >> 8) | (*(in++) << 24);
	decomp_size = gbaheader >> 8;

	out = (u8*) alloc_from_heap(decomp_size);
	if(!out)
		fatal_error("LZDecode: Not enough memory to allocate %d bytes!\n", decomp_size);
	*output = out;

	for(i = 0; i < N - F; i++)
		text_buf[i] = 0xFF;
	r = N - F;
	flags = z = 7;
	while(cur_size < decomp_size) {
		flags <<= 1;
		z++;
		if (z == 8) {				// read new block flag
			c = *(in++);
			flags = c;
			z = 0;				// reset counter
		}
		if(!(flags & 0x80)) {			// flag bit zero => uncompressed
			c = *(in++);
			if(cur_size < decomp_size)
				*(out++) = c;
			text_buf[r++] = c;
			r &= N - 1;
			cur_size++;
		} else {
			i = *(in++);
			j = *(in++);
			j = j | ((i << 8) & 0xf00);		// match offset
			i = ((i >> 4) & 0x0f) + THRESHOLD;	// match length
			for (k = 0; k <= i; k++) {
				c = text_buf[(r - j - 1) & (N - 1)];
				if(cur_size < decomp_size)
					*(out++) = c;
				text_buf[r++] = c;
				r &= N - 1;
				cur_size++;
			}
		}
	}

	return decomp_size;
}
