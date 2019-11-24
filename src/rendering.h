#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <glad/glad.h>

using MeshID = uint8_t;

struct MeshDrawInfo {
    GLuint vao;
    GLuint element_count;
};

struct MeshManager;

MeshManager* init_mesh_manager();
MeshID next_mesh(MeshManager* manager);
void populate_quad_mesh(MeshManager* manager, MeshID id);
void draw_mesh(MeshDrawInfo info, GLuint shader_program);
MeshDrawInfo prepare_mesh_for_drawing(MeshManager* manager, MeshID id);
void free_mesh_manager(MeshManager* manager);