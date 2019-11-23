#define GLEW_STATIC

#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <glm/ext/vector_int2.hpp>
#include FT_FREETYPE_H

struct Character {
	GLuint tex_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

GLFWwindow* initialize_window();
void glfw_error_callback(int error, const char* desc);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
void render_frame(GLFWwindow* window);
Character* create_font_textures(char*);
void directory_change_callback(PVOID param, BOOLEAN timed_out);