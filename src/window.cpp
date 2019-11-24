#include "window.h"


const int WIDTH = 800;
const int HEIGHT = 600;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void begin_frame(Window window) {
    // Process input
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


void glfw_error_callback(int error, const char* desc) {
	printf("Glfw error %s\n", desc);
}


Window init_window()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		printf("Failed to initialize GLFW");
		return nullptr;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Compositor", 0, 0);
	if (!window) {
		printf("Failed to create window.");
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	// vsync
	glfwSwapInterval(1);

	if (!gladLoadGL()) {
		printf("Failed to load OpenGL.\n");
		glfwDestroyWindow(window);
		glfwTerminate();
		return nullptr;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}


void end_frame(Window window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}


bool should_close(Window window) {
    return glfwWindowShouldClose(window);
}


void free_window(Window window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
