#pragma once 

typedef unsigned int ShaderID;

struct ShaderManager;

ShaderManager* init_shader_manager();
ShaderID shader_from_source(ShaderManager* manager, const char* filename, unsigned int kind);
ShaderID link_program(ShaderManager* manager, ShaderID vtx, ShaderID frg);
void hotreload_all_shaders(ShaderManager* manager);
unsigned int handle_for_program(ShaderManager* manager, ShaderID id);
void free_shader_manager(ShaderManager* manager);
