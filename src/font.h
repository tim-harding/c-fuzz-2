#pragma once

namespace Fonts {
	const char ASCII_START = 32; // Space
	const char ASCII_END = 126;  // ~
	const char ASCII_CHAR_COUNT = ASCII_END - ASCII_START;

	struct Character {
    	// x distance from character origin to next character in 1/64 pixels
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

	// Get rid of this and just use the allocator
	const int MAX_FONTS = 1 << 4;
	struct FontManager {
		int count = 0;
		Font fonts[MAX_FONTS];
	};

	typedef unsigned int FontHandle;

	FontManager* init_fonts();
	FontHandle create_font(FontManager* manager, char* font_file, int point_size);
	unsigned int tex_handle_for_font(FontManager* manager, FontHandle handle);
}
