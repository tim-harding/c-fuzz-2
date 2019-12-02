#include "memory.h"


namespace Memory {
	gbAllocator create_allocator(int size) {
		gbArena* arena = (gbArena*)calloc(sizeof(gbArena), 1);
		void* backing = malloc(size);
		gb_arena_init_from_memory(arena, backing, size);
		return gb_arena_allocator(arena);
	}

	Storage init(int permanent_size, int scratch_size) {
		Storage storage = {0};
		storage.permanent = create_allocator(permanent_size);
		storage.scratch   = create_allocator(scratch_size);
		return storage;
	}

	void free_allocator(gbAllocator allocator) {
		gbArena* arena = (gbArena*)allocator.data;
		free(arena->physical_start);
	}

	void free_storage(Storage storage) {
		free_allocator(storage.permanent);
		free_allocator(storage.scratch);
	}

	inline int kilobytes(int bytes) {
		return bytes * 1024;
	}

	inline int megabytes(int kilos) {
		return kilobytes(kilos) * 1024;
	}

	inline int gigabytes(int gigs) {
		return megabytes(gigs) * 1024;
	}
}
