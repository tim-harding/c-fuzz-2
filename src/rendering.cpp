#include "rendering.h"
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include "glad/glad.h"

// TODO: remove malloc and free calls

namespace Rendering {
	Mesh* create_mesh(Memory::Storage storage) {
		Mesh* mesh = gb_alloc_item(storage.permanent, Mesh);
		gb_array_init(mesh->vertices, storage.permanent);
		gb_array_init(mesh->indices, storage.permanent);
		return mesh;
	}

	void populate_quad_mesh(Memory::Storage storage, Mesh* mesh) {
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

    	int vertex_count = sizeof(vertices) / sizeof(float);
    	gb_array_resize(mesh->vertices, vertex_count);
    	memcpy(mesh->vertices, vertices, sizeof(vertices));

    	int index_count = sizeof(indices) / sizeof(GLuint);
    	gb_array_resize(mesh->indices, index_count);
    	memcpy(mesh->indices, indices, sizeof(indices));
	}


	void draw_mesh(Mesh* mesh, GLuint shader_program) {
    	glUseProgram(shader_program);
    	glBindVertexArray(mesh->vao);
    	int count = gb_array_count(mesh->indices);
    	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	}


	void prepare_mesh_for_drawing(Mesh* mesh) {
    	GLuint vao;
    	glGenVertexArrays(1, &vao);
    	glBindVertexArray(vao);

    	GLuint vbo;
    	glGenBuffers(1, &vbo);
    	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    	// TODO: Only allows for a single vertex attribute at the moment. 
    	int vertex_count = gb_array_count(mesh->vertices);
    	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), mesh->vertices, GL_STATIC_DRAW);

    	GLuint ebo;
    	glGenBuffers(1, &ebo);
    	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    	int index_count = gb_array_count(mesh->indices);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLuint), mesh->indices, GL_STATIC_DRAW);

		// should use glgetattriblocation
		const int pos_a = 0;
		const int uv_a  = 1;
    	glVertexAttribPointer(pos_a, 2, GL_FLOAT, false, 4 * sizeof(float), nullptr);
    	glEnableVertexAttribArray(pos_a); 
    	glVertexAttribPointer(uv_a, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    	glEnableVertexAttribArray(uv_a);

    	mesh->vao = vao;
	}
}
