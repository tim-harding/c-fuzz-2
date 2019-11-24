#include "shader.h"


const int BUFFER_SIZE = 1 << 16;
const int MAX_SHADERS = 255;
const int MAX_PROGRAMS = 255;


using Timestamp = uint64_t;


struct Shader {
    bool valid;
	GLuint handle;
    Timestamp timestamp;
    const char* filename;

    uint8_t program_count;
    ShaderID* programs;
};


struct Program {
    bool valid;
    GLuint handle;
};


struct ShaderManager {
    uint8_t shader_count;
    uint8_t program_count;
    Shader shaders[MAX_SHADERS];
    Program programs[MAX_PROGRAMS];
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


GLuint handle_for_program(ShaderManager* manager, ShaderID id) {
    return manager->programs[id].handle;
}


void gl_relink_program(Program& program) {
	GLuint handle = program.handle;
	glLinkProgram(handle);

	int success;
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (!success) {
		printf("Failed to link shader program.\n");
	}
	program.valid = success;
}


bool source_shader(GLuint shader, const char* filename) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		printf("Could not open shader file: %s\n", filename);
		return false;
	}

	char* buffer = (char*)malloc(BUFFER_SIZE * sizeof(char*));
	size_t bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file);
	if (bytes_read == BUFFER_SIZE) {
		printf("Shader program requires a larger text buffer.");
		return false;
	}
	fclose(file);
	// fread does not null-terminate the string.
	buffer[bytes_read] = '\0';
	glShaderSource(shader, 1, &buffer, nullptr);
	free(buffer);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
        printf("Shader compilation error for %s:\n%s\n", filename, info_log);
		return false;
	}
	
	return true;
}


void attempt_shader_hotreload(ShaderManager* manager, Shader& shader) {
	Timestamp timestamp = file_timestamp(shader.filename);
	if (shader.timestamp < timestamp) {
		shader.valid = source_shader(shader.handle, shader.filename);
	}

	if (shader.valid) {
		for (int i = 0, count = shader.program_count; i < count; i++) {
            ShaderID id = shader.programs[i];
			gl_relink_program(manager->programs[id]);
		}
	}
}


void init_program(ShaderManager* manager, ShaderID id, ShaderID vtx_id, ShaderID frg_id) {
    Shader& vtx = manager->shaders[vtx_id];
    Shader& frg = manager->shaders[frg_id];
    Program& program = manager->programs[id];

	GLuint handle = glCreateProgram();
	glAttachShader(handle, vtx.handle);
	glAttachShader(handle, frg.handle);
	gl_relink_program(program);

	vtx.programs[vtx.program_count++] = id;
	frg.programs[frg.program_count++] = id;

	program.handle = handle;
}


void init_shader(Shader& out, const char* filename, GLenum kind) {
	GLuint shader = glCreateShader(kind);
	out.valid = source_shader(shader, filename);
	out.timestamp = file_timestamp(filename);
	out.handle = shader;
	out.filename = filename;
	out.program_count = 0;
	out.programs = (ShaderID*)malloc(MAX_SHADERS * sizeof(ShaderID));
}


ShaderManager* init_shader_manager() {
    ShaderManager* manager = (ShaderManager*)malloc(sizeof(ShaderManager));
    manager->program_count = 0;
    manager->shader_count = 0;
    return manager;
}


// TODO: Fails if more than MAX_SHADERS are used, have some
// sort of error return. 
ShaderID shader_from_source(ShaderManager* manager, const char* filename, GLuint kind) {
    uint8_t id = manager->shader_count;
    Shader& shader = manager->shaders[id];
    init_shader(shader, filename, kind);
    manager->shader_count++;
    return id;
}


// TODO: See not note on shader_from_source
ShaderID link_program(ShaderManager* manager, ShaderID vtx_id, ShaderID frg_id) {
    uint8_t id = manager->program_count;
    Program& program = manager->programs[id];
    init_program(manager, id, vtx_id, frg_id);
    manager->program_count++;
    return id;
}


void hotreload_all_shaders(ShaderManager* manager) {
    for (int i = 0, count = manager->shader_count; i < count; i++) {
        Shader& shader = manager->shaders[i];
        attempt_shader_hotreload(manager, shader);
    }
}


void free_shader_manager(ShaderManager* manager) {
    for (int i = 0, shader_count = manager->shader_count; i < shader_count; i++) {
        Shader shader = manager->shaders[i];
        glDeleteShader(shader.handle);
        free(shader.programs);
		// `filename` is a pointer but we don't free it because
		// it isn't dynamically allocated. 
    }
    
    for (int i = 0, program_count = manager->program_count; i < program_count; i++) {
        Program program = manager->programs[i];
        glDeleteProgram(program.handle);
    }

    free(manager);
}