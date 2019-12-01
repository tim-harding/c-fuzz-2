#include "fonts.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"

#define GB_IMPLEMENTATION
#include "gb.h"

#include "glad/glad.h"

// TODO: 
// - Materials, dealing with attributes and uniforms


int main(int argc, char** argv) {
	const int ALLOC_SIZE = 1 << 16;
	void* block = malloc(ALLOC_SIZE);
	gbArena arena = {0};
	gb_arena_init_from_memory(&arena, block, ALLOC_SIZE);
	gbAllocator allocator = gb_arena_allocator(&arena);

	Window::Handle window = Window::init();
	if (!window) {
		return -1;
	}

	Fonts::Font* font = gb_alloc_item(allocator, Fonts::Font);
	Fonts::from_file(font, "fonts/cascadia.ttf", 36);

	Shader::Manager* shader_manager = Shader::init_shader_manager();
	Shader::Id vtx = Shader::shader_from_source(shader_manager, "shaders/tri_test_vtx.glsl", GL_VERTEX_SHADER);
	Shader::Id frg = Shader::shader_from_source(shader_manager, "shaders/tri_test_frg.glsl", GL_FRAGMENT_SHADER);
	Shader::Id program = Shader::link_program(shader_manager, vtx, frg);

	Rendering::MeshManager* mesh_manager = Rendering::init_mesh_manager();
	Rendering::MeshID mesh = Rendering::next_mesh(mesh_manager);
	Rendering::populate_quad_mesh(mesh_manager, mesh);
	Rendering::prepare_mesh_for_drawing(mesh_manager, mesh);

	while (!Window::should_close(window)) {
		Window::begin_frame(window);

		Shader::hotreload_all_shaders(shader_manager);
		glBindTexture(GL_TEXTURE_2D, font->tex);
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		Shader::GlHandle shader = Shader::handle_for_program(shader_manager, program);
		Rendering::draw_mesh(mesh_manager, mesh, shader);

		Window::end_frame(window);
	}

	Shader::free_shader_manager(shader_manager);
	Window::free(window);
	Rendering::free_mesh_manager(mesh_manager);

	return 0;
}
