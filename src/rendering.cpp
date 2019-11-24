#include "rendering.h"

const int MAX_MESHES = 256;

struct Mesh {
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
         0.5f,  0.5f,
         0.5f, -0.5f,
        -0.5f, -0.5f, 
        -0.5f,  0.5f
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


void draw_mesh(MeshDrawInfo info, GLuint shader_program) {
    glUseProgram(shader_program);
    glBindVertexArray(info.vao);
    glDrawElements(GL_TRIANGLES, info.element_count, GL_UNSIGNED_INT, 0);
}


MeshDrawInfo prepare_mesh_for_drawing(MeshManager* manager, MeshID id) {
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

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0); // should use glgetattriblocation

    MeshDrawInfo info;
    info.vao = vao;
    info.element_count = mesh.index_count;
    return info;
}