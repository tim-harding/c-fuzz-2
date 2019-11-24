#include "main.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int main(int argc, char** argv) {
	GLFWwindow* window = initialize_window();
	if (!window) {
		return -1;
	}

	Character* characters = create_font_textures("fonts/Cascadia.ttf");
	if (!characters) {
		return -1;
	}

	int MAX_CWD_SIZE = 256;
	char* cwd = (char*)malloc(sizeof(char) * MAX_CWD_SIZE);
	_getcwd(cwd, MAX_CWD_SIZE);

	printf("CWD: %s\n", cwd);

	HANDLE cwd_handle = CreateFileA(
		cwd, 
		GENERIC_READ, 
		// Allow all operations by other users of the file.
		// We want to watch changes, not block access.
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		// Default security
		nullptr, 
		OPEN_EXISTING,
		// Used to create a handle for ReadDirectoryChangesW
		FILE_FLAG_BACKUP_SEMANTICS,
		// Ignored when opening existing
		nullptr
	);

	if (cwd_handle == INVALID_HANDLE_VALUE) {
		printf("Could not get a handle to the working directory.");
		return -1;
	}

	DWORD filter = FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_LAST_WRITE;
	DWORD buffer_size = 12 * sizeof(FILE_NOTIFY_INFORMATION);
	LPVOID buffer = malloc(buffer_size);
	LPDWORD bytes_returned = nullptr;
	bool success = ReadDirectoryChangesW(
		cwd_handle,
		buffer,
		buffer_size,
		true,
		filter,
		bytes_returned,
		// Last parameters relevant for async calls
		nullptr,
		nullptr
	);

	if (success) {
		printf("%d\n", *bytes_returned);
	} else {
		printf("Could not read directory changes.\n");
	}

	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	free(buffer);
	free(characters);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


GLFWwindow* initialize_window()
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


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void glfw_error_callback(int error, const char* desc) {
	printf("Glfw error %s\n", desc);
}

Character* create_font_textures(char* font_file) {
	FT_Library ft;
	int failure = FT_Init_FreeType(&ft);
	if (failure) {
		printf("Failed to initialize FreeType\n");
		return nullptr;
	}
	
	FT_Face face;
	failure = FT_New_Face(ft, font_file, 0, &face);
	if (failure) {
		printf("Failed to load font\n");
		FT_Done_FreeType(ft);
		return nullptr;
	}

	// Width calculated dynamically
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Most textures' dimensions are a multiple of four,
	// but we want to loosen that restriction for the moment.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	Character* characters = (Character*)malloc(128 * sizeof(Character));
	for (GLubyte i = 0; i < 128; i++) {
		// Calls FT_Render_Glyph after font is loaded
		failure = FT_Load_Char(face, i, FT_LOAD_RENDER);
		if (failure != 0) {
			printf("Failed to load glyph: %c", i);
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			free(characters);
			return nullptr;
		}

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		FT_GlyphSlot glyph = face->glyph;
		Character* c = &characters[i];
		c->tex_id = tex;
		c->size = glm::ivec2(glyph->bitmap.width, glyph->bitmap.rows);
		c->bearing = glm::ivec2(glyph->bitmap_left, glyph->bitmap_top);
		c->advance = glyph->advance.x;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			c->size.x,
			c->size.y,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return characters;
}
