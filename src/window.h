#pragma once

namespace Window {
	const int WIDTH = 800;
	const int HEIGHT = 600;

	// We're not including our windowing header here, 
	// so this handle is just going to be a void pointer
	// that we cast to the correct type in the implementation.
	using Handle = void*;

	Handle init_window();
	void free(Handle window);
	bool should_close(Handle window);
	void begin_frame(Handle window);
	void end_frame(Handle window);
}
