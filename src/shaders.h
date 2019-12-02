#pragma once 
#include "memory.h"

namespace Shaders {
	using Handle = void*;
	using Id = unsigned int;
	using GlHandle = unsigned int;
	using GlEnum = unsigned int;
	using StaticCstr = const char*;

	Handle init(Memory::Storage storage);
	Id stage_from_source(Handle handle, StaticCstr filename, GlEnum kind);
	Id link_program(Handle handle, Id vtx, Id frg);
	GlHandle handle_for_program(Handle handle, Id id);
	void hotreload(Handle handle);
	void free(Handle handle);
}
