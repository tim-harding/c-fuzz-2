#include "rendering.h"
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <glad/glad.h>

const int MAX_MESHES = 256;

struct Mesh {
	GLuint vao;

    GLuint vertex_count;
    float* vertices;

    GLuint index_count;
    GLuint* indices;
};


struct MeshManager {
    uint8_t count;
    Mesh meshes[MAX_MESHES];
};


MeshManager* init_mesh_manager() {
    MeshManager* manager = (MeshManager*)malloc(sizeof(MeshManager));
    manager->count = 0;
    return manager;
}


void free_mesh_manager(MeshManager* manager) {
    for (int i = 0, count = manager->count; i < count; i++) {
        Mesh mesh = manager->meshes[i];
        free(mesh.indices);
        free(mesh.vertices);
    }
    free(manager);
}


MeshID next_mesh(MeshManager* manager) {
    return manager->count++;
}


void populate_quad_mesh(MeshManager* manager, MeshID id) {
    float vertices[] = {
      // World coords      UV coords
         0.5f,  0.5f,      1.f, 1.f,
         0.5f, -0.5f,      1.f, 0.f,
        -0.5f, -0.5f,      0.f, 0.f,
        -0.5f,  0.5f,      0.f, 1.f
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3   
    };
    
    Mesh& mesh = manager->meshes[id];

    mesh.vertex_count = sizeof(vertices) / sizeof(float);
    mesh.vertices = (float*)malloc(sizeof(vertices));
    memcpy(mesh.vertices, vertices, sizeof(vertices));

    mesh.index_count = sizeof(indices) / sizeof(GLuint);
    mesh.indices = (GLuint*)malloc(sizeof(indices)); 
    memcpy(mesh.indices, indices, sizeof(indices));
}


void draw_mesh(MeshManager* manager, MeshID id, GLuint shader_program) {
	Mesh mesh = manager->meshes[id];
    glUseProgram(shader_program);
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
}


void prepare_mesh_for_drawing(MeshManager* manager, MeshID id) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    Mesh& mesh = manager->meshes[id];
    // TODO: Only allows for a single vertex attribute at the moment. 
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(float), mesh.vertices, GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(GLuint), mesh.indices, GL_STATIC_DRAW);

	// should use glgetattriblocation
	const int pos_a = 0;
	const int uv_a  = 1;
    glVertexAttribPointer(pos_a, 2, GL_FLOAT, false, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(pos_a); 
    glVertexAttribPointer(uv_a, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(uv_a);

    mesh.vao = vao;
}
