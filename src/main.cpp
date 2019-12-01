#include "font.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct ImageInfo {
	int width;
	int height;
	int channels;
};


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

	const char* FONT_TEXTURE_FILENAME = "fonts/out.png";
	const char* FONT_SIDECAR_FILENAME = "fonts/sidecar.json";

	ImageInfo image_info;
	unsigned char* data = stbi_load(FONT_TEXTURE_FILENAME, &image_info.width, &image_info.height, &image_info.channels, 0);
	if (!data) {
		printf("Failed to load image: %s\n", FONT_TEXTURE_FILENAME);
		return -1;
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		image_info.width,
		image_info.height,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		data
	);

	stbi_image_free(data);

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
