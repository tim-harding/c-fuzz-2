#pragma once 

#include <sys/stat.h>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <cstring>
#include <assert.h>

using ShaderID = uint8_t;
using ProgramID = uint8_t;

const int MAX_BUFFERS = 8;
const int BUFFER_SIZE = 1 << 16;
const int MAX_SHADERS = 255;
const int MAX_PROGRAMS = 255;

struct Shader {
    bool valid;
	GLuint handle;
    uint64_t timestamp;
    char* filename;

    uint8_t program_count;
    ProgramID* programs;
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

ShaderManager* alloc_and_init_shader_manager();
ShaderID shader_from_source(ShaderManager* manager, char* filename, GLuint kind);
ProgramID link_program(ShaderManager* manager, ShaderID vtx, ShaderID frg);
void hotreload_all_shaders(ShaderManager* manager);
void free_shader_manager(ShaderManager* manager);