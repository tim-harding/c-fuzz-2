#pragma once 
#include <sys/stat.h>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <cstring>

using ShaderID = uint8_t;

struct ShaderManager;

ShaderManager* init_shader_manager();
ShaderID shader_from_source(ShaderManager* manager, const char* filename, GLuint kind);
ShaderID link_program(ShaderManager* manager, ShaderID vtx, ShaderID frg);
void hotreload_all_shaders(ShaderManager* manager);
GLuint handle_for_program(ShaderManager* manager, ShaderID id);
void free_shader_manager(ShaderManager* manager);