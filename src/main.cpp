#include "font.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"


int main(int argc, char** argv) {
	Window window = init_window();
	if (!window) {
		return -1;
	}

	FontManager* font_manager = init_font_manager();
	FontSuccess cascadia = add_font(font_manager, "fonts/cascadia.ttf");
	if (!cascadia.ok) {
		return -1;
	}

	ShaderManager* shader_manager = init_shader_manager();
	ShaderID vtx = shader_from_source(shader_manager, "shaders/tri_test_vtx.glsl", GL_VERTEX_SHADER);
	ShaderID frg = shader_from_source(shader_manager, "shaders/tri_test_frg.glsl", GL_FRAGMENT_SHADER);
	ShaderID program = link_program(shader_manager, vtx, frg);

	MeshManager* mesh_manager = init_mesh_manager();
	MeshID mesh = next_mesh(mesh_manager);
	populate_quad_mesh(mesh_manager, mesh);
	MeshDrawInfo info = prepare_mesh_for_drawing(mesh_manager, mesh);
	
	while (!should_close(window)) {
		begin_frame(window);

		hotreload_all_shaders(shader_manager);
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		draw_mesh(info, handle_for_program(shader_manager, program));

		end_frame(window);
	}

	free_shader_manager(shader_manager);
	free_window(window);
	free_font_manager(font_manager);
	free_mesh_manager(mesh_manager);

	return 0;
}