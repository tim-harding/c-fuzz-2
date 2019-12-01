#include "font.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"

#define GB_IMPLEMENTATION
#include "gb.h"

#include <glad/glad.h>

// TODO: 
// - Materials, dealing with attributes and uniforms

int main(int argc, char** argv) {
	Window window = init_window();
	if (!window) {
		return -1;
	}

	Fonts* fonts = init_fonts();
	FontHandle cascadia = create_font(fonts, "fonts/cascadia.ttf", 36);	

	ShaderManager* shader_manager = init_shader_manager();
	ShaderID vtx = shader_from_source(shader_manager, "shaders/tri_test_vtx.glsl", GL_VERTEX_SHADER);
	ShaderID frg = shader_from_source(shader_manager, "shaders/tri_test_frg.glsl", GL_FRAGMENT_SHADER);
	ShaderID program = link_program(shader_manager, vtx, frg);

	MeshManager* mesh_manager = init_mesh_manager();
	MeshID mesh = next_mesh(mesh_manager);
	populate_quad_mesh(mesh_manager, mesh);
	prepare_mesh_for_drawing(mesh_manager, mesh);

	while (!should_close(window)) {
		begin_frame(window);

		hotreload_all_shaders(shader_manager);
		glBindTexture(GL_TEXTURE_2D, tex_handle_for_font(fonts, cascadia));
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		GLuint shader = handle_for_program(shader_manager, program);
		draw_mesh(mesh_manager, mesh, shader);

		end_frame(window);
	}

	free_shader_manager(shader_manager);
	free_window(window);
	free_mesh_manager(mesh_manager);

	return 0;
}
