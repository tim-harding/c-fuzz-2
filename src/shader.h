#pragma once 

namespace Shader {
	const int BUFFER_SIZE = 1 << 16;
	const int MAX_SHADERS = 255;
	const int MAX_PROGRAMS = 255;

	using Timestamp = unsigned long;
	using Id = unsigned int;
	using GlHandle = unsigned int;
	using GlEnum = unsigned int;
	using StaticCstr = const char*;

	struct Stage {
    	bool valid;
		GlHandle handle;
    	Timestamp timestamp;
    	StaticCstr filename;

    	int program_count;
    	Id * programs;
	};

	struct Program {
    	bool valid;
    	GlHandle handle;
	};

	struct Manager {
    	int shader_count;
    	int program_count;
    	Stage shaders[MAX_SHADERS];
    	Program programs[MAX_PROGRAMS];
	};

	struct Manager;

	Manager* init_shader_manager();
	Id shader_from_source(Manager* manager, StaticCstr filename, GlEnum kind);
	Id link_program(Manager* manager, Id vtx, Id frg);
	void hotreload_all_shaders(Manager* manager);
	GlHandle handle_for_program(Manager* manager, Id id);
	void free_shader_manager(Manager* manager);
}
