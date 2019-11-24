#include "font.h"
#include "shader.h"
#include "window.h"


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
	ShaderID vtx = shader_from_source(shader_manager, "shaders/text_vtx.glsl", GL_VERTEX_SHADER);
	ShaderID frg = shader_from_source(shader_manager, "shaders/text_frg.glsl", GL_FRAGMENT_SHADER);
	ShaderID program = link_program(shader_manager, vtx, frg);

	while (!should_close(window)) {
		begin_frame(window);

		hotreload_all_shaders(shader_manager);
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		end_frame(window);
	}

	free_shader_manager(shader_manager);
	free_window(window);
	free_font_manager(font_manager);

	return 0;
}