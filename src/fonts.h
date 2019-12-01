#pragma once

namespace Fonts {
	const char ASCII_START = 32; // Space
	const char ASCII_END = 126;  // ~
	const char ASCII_CHAR_COUNT = ASCII_END - ASCII_START;

	struct Character {
    	// x pixels from character origin to next character
		long advance;

    	// In pixels
    	unsigned int w;
    	unsigned int h;

    	// pixels from origin to left edge of character BB
    	int bearing_x; 
    	// pixels from baseline to top edge of character BB
    	int bearing_y; 
	};

	struct Font {
		unsigned int tex;
		Character characters[ASCII_CHAR_COUNT];
	};

	void from_file(Font* out, char* font_file, int point_size);
}
