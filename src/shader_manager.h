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

struct ShaderManager;

ShaderManager* alloc_and_init_shader_manager();
ShaderID shader_from_source(ShaderManager* manager, char* filename, GLuint kind);
ProgramID link_program(ShaderManager* manager, ShaderID vtx, ShaderID frg);
void hotreload_all_shaders(ShaderManager* manager);
GLuint handle_for_program(ShaderManager* manager, ProgramID id);
void free_shader_manager(ShaderManager* manager);