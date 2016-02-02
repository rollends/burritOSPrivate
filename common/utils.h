#ifndef UTILS_H
#define UTILS_H

/// Borrowed from Standford's Bit Twiddling hacks
static inline U8 fastLog2( U32 v )
{
	static const int DeBruijnTable[32] = 
	{
	  	0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
	  	8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
	};

	v |= v >> 1; // first round down to one less than a power of 2 
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;

	return DeBruijnTable[(v * 0x07C4ACDDU) >> 27];
}

#endif
