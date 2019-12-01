#pragma once

// We're not including our windowing header here, 
// so this handle is just going to be a void pointer
// that we cast to the correct type in the implementation.
typedef void* Window;

Window init_window();
void free_window(Window window);
bool should_close(Window window);
void begin_frame(Window window);
void end_frame(Window window);
