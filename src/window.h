#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>


using Window = GLFWwindow*;


Window init_window();
void free_window(Window window);
bool should_close(Window window);
void begin_frame(Window window);
void end_frame(Window window);