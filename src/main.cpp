#include "main.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int main(int argc, char** argv) {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Compositor", nullptr, nullptr);
	if (!window) {
		printf("Failed to create window.");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// vsync
	glfwSwapInterval(1);

	if (!gladLoadGL()) {
		printf("Failed to load OpenGL.");
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		render_frame(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


void render_frame(GLFWwindow* window) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Here is where you would render whatever
	// non-UI stuff you need

	ImGui::Begin("Demo window");
	ImGui::Button("Hello!");
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void glfw_error_callback(int error, const char* desc) {
	printf("Glfw error %s", desc);
}
