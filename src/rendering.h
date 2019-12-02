#pragma once
#include "memory.h"

namespace Rendering {
	const int MAX_MESHES = 256;

	typedef unsigned int GlHandle;

	struct Mesh {
		GlHandle vao;
    	gbArray(float) vertices;
    	gbArray(GlHandle) indices;
	};

	Mesh* create_mesh(Memory::Storage storage);
	void populate_quad_mesh(Memory::Storage storage, Mesh* mesh);
	void prepare_mesh_for_drawing(Mesh* mesh);
	void draw_mesh(Mesh* mesh, unsigned int shader_program);
}
