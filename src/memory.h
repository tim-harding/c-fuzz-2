#pragma once 

#include <gb/gb.h>

namespace Memory {
	struct Storage {
		gbAllocator permanent;
		gbAllocator scratch;
	};

	Storage init(int permanent_size, int scratch_size);

	inline int kilobytes(int bytes);
	inline int megabytes(int kilos);
	inline int gigabytes(int gigs);
}
