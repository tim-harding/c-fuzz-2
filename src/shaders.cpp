#include "shaders.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include "glad/glad.h"

namespace Shaders {
	using Timestamp = uint64_t;

	struct Program {
    	bool valid;
    	GlHandle handle;
	};

	struct Stage {
    	bool valid;
		GlHandle handle;
    	Timestamp timestamp;
    	StaticCstr filename;
    	gbArray(Program*) programs;
	};

	struct Manager {
		Memory::Storage storage;
    	gbArray(Stage*) stages;
    	gbArray(Program*) programs;
	};

	Timestamp file_timestamp(const char* filename) {
		Timestamp timestamp;
		struct __stat64 file_info;
		int success = _stat64(filename, &file_info);
		if (success == 0) {
			timestamp = file_info.st_mtime;
		}
		return timestamp;
	}

	void gl_relink_program(Program* program) {
		GLuint handle = program->handle;
		glLinkProgram(handle);

		int success;
		glGetProgramiv(handle, GL_LINK_STATUS, &success);
		if (!success) {
			printf("Failed to link shader program.\n");
		}
		program->valid = success;
	}

	bool source_shader(Memory::Storage storage, GLuint handle, const char* filename) {
		FILE* file = fopen(filename, "r");
		if (!file) {
			printf("Could not open shader file: %s\n", filename);
			return false;
		}

		const int BUFFER_SIZE = 1 << 16;
		char* buffer = gb_alloc_array(storage.scratch, char, BUFFER_SIZE);
		size_t bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file);
		if (bytes_read == BUFFER_SIZE) {
			printf("Shader program requires a larger text buffer.");
			return false;
		}
		fclose(file);
		// fread does not null-terminate the string.
		buffer[bytes_read] = '\0';
		glShaderSource(handle, 1, &buffer, nullptr);

		gb_free_all(storage.scratch);

		glCompileShader(handle);

		int success;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
		if (!success) {
			char info_log[512];
			glGetShaderInfoLog(handle, sizeof(info_log), nullptr, info_log);
        	printf("Shader compilation error for %s:\n%s\n", filename, info_log);
			return false;
		}

		return true;
	}


	void init_program(Manager* manager, Program* program, Id vtx_id, Id frg_id) {
    	Stage* vtx = manager->stages[vtx_id];
    	Stage* frg = manager->stages[frg_id];

		GLuint handle = glCreateProgram();
		glAttachShader(handle, vtx->handle);
		glAttachShader(handle, frg->handle);
		program->handle = handle;
		gl_relink_program(program);

		gb_array_append(vtx->programs, program);
		gb_array_append(frg->programs, program);
	}


	void init_shader(Memory::Storage storage, Stage* out, const char* filename, GLenum kind) {
		GLuint stage = glCreateShader(kind);
		out->valid = source_shader(storage, stage, filename);
		out->timestamp = file_timestamp(filename);
		out->handle = stage;
		out->filename = filename;
		gb_array_init(out->programs, storage.permanent);
	}


	Handle init(Memory::Storage storage) {
    	Manager* manager = gb_alloc_item(storage.permanent, Manager);
    	gb_array_init(manager->stages, storage.permanent);
    	gb_array_init(manager->programs, storage.permanent);
    	manager->storage = storage;
    	return (Handle)manager;
	}

	Id stage_from_source(Handle handle, StaticCstr filename, GlEnum kind) {
		Manager* manager = (Manager*)handle;
    	int id = gb_array_count(manager->stages);
    	Stage* stage = gb_alloc_item(manager->storage.permanent, Stage);
    	init_shader(manager->storage, stage, filename, kind);
    	gb_array_append(manager->stages, stage);
    	return id;
	}

	Id link_program(Handle handle, Id vtx_id, Id frg_id) {
		Manager* manager = (Manager*)handle;
    	int id = gb_array_count(manager->programs);
    	Program* program = gb_alloc_item(manager->storage.permanent, Program);
    	init_program(manager, program, vtx_id, frg_id);
    	gb_array_append(manager->programs, program);
    	return id;
	}

	void attempt_shader_hotreload(Manager* manager, Stage* stage) {
		Timestamp timestamp = file_timestamp(stage->filename);
		if (stage->timestamp < timestamp) {
			Memory::Storage storage = manager->storage;
			bool valid = source_shader(storage, stage->handle, stage->filename);
			gb_free_all(storage.scratch);
			stage->valid = valid;
			if (valid) {
				int count = gb_array_count(stage->programs);
				for (int i = 0; i < count; i++) {
            		Program* program = stage->programs[i];
					gl_relink_program(program);
				}
			}
		}
	}

	void hotreload(Handle handle) {
		Manager* manager = (Manager*)handle;
		int count = gb_array_count(manager->stages);
    	for (int i = 0; i < count; i++) {
        	Stage* stage = manager->stages[i];
        	attempt_shader_hotreload(manager, stage);
    	}
	}

	GlHandle handle_for_program(Handle handle, Id id) {
		Manager* manager = (Manager*)handle;
		Program* program = manager->programs[id];
		return program->handle;
	}
}
