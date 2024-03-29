#include "fonts.h"
#include "shaders.h"
#include "window.h"
#include "rendering.h"
#include "memory.h"

#define GB_IMPLEMENTATION
#include <gb/gb.h>

#include "glad/glad.h"

#include <cstdio>

// TODO: 
// - Materials, dealing with attributes and uniforms


int main(int argc, char** argv) {
	Memory::Storage storage = Memory::init(Memory::megabytes(1), Memory::megabytes(5));

	Window::Handle window = Window::init();
	if (!window) {
		return -1;
	}

	Fonts::Font* cascadia = Fonts::from_file(storage, "fonts/cascadia.ttf", 36);

	Shaders::Handle shaders = Shaders::init(storage);
	Shaders::Id vtx = Shaders::stage_from_source(shaders, "shaders/tri_test_vtx.glsl", GL_VERTEX_SHADER);
	Shaders::Id frg = Shaders::stage_from_source(shaders, "shaders/tri_test_frg.glsl", GL_FRAGMENT_SHADER);
	Shaders::Id program = Shaders::link_program(shaders, vtx, frg);

	Rendering::Mesh* mesh = Rendering::create_mesh(storage);
	Rendering::populate_quad_mesh(storage, mesh);
	Rendering::prepare_mesh_for_drawing(mesh);

	while (!Window::should_close(window)) {
		Window::begin_frame(window);

		Shaders::hotreload(shaders);
		glBindTexture(GL_TEXTURE_2D, cascadia->tex);
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		Shaders::GlHandle shader = Shaders::handle_for_program(shaders, program);
		Rendering::draw_mesh(mesh, shader);

		Window::end_frame(window);
	}

	Window::free_window(window);
	Memory::free_storage(storage);

	return 0;
}
