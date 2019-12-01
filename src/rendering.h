#pragma once

namespace Rendering {
	const int MAX_MESHES = 256;

	typedef unsigned int GlHandle;
	typedef int MeshID;
	typedef struct MeshManager MeshManager;

	struct Mesh {
		GlHandle vao;

    	int vertex_count;
    	float* vertices;

    	int index_count;
    	GlHandle* indices;
	};

	struct MeshManager {
    	int count;
    	Mesh meshes[MAX_MESHES];
	};

	MeshManager* init_mesh_manager();
	MeshID next_mesh(MeshManager* manager);
	void populate_quad_mesh(MeshManager* manager, MeshID id);
	void draw_mesh(MeshManager* manager, MeshID mesh, unsigned int shader_program);
	void prepare_mesh_for_drawing(MeshManager* manager, MeshID id);
	void free_mesh_manager(MeshManager* manager);
}
