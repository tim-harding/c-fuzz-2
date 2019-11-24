#include "font.h"


const int MAX_FONTS = 255;
const int ASCII_CHAR_COUNT = 128;


struct Character {
	GLuint tex_id;

    // x distance from character origin to next character in 1/64 pixels
	GLuint advance;

    // In pixels
    uint8_t width;
    uint8_t height;

    // pixels from origin to left edge of character BB
    uint8_t bearing_x; 
    // pixels from baseline to top edge of character BB
    uint8_t bearing_y; 
};


struct Font {
	// Array of characters, indexed by ASCII value
	Character characters[ASCII_CHAR_COUNT];
};


struct FontManager {
	uint8_t font_count;
	Font* fonts[MAX_FONTS];
};


// `out` should be an array of length ASCII_CHAR_COUNT
bool create_font_textures(char* font_file, Character* out) {
	FT_Library ft;
	int failure = FT_Init_FreeType(&ft);
	if (failure) {
		printf("Failed to initialize FreeType\n");
		return false;
	}
	
	FT_Face face;
	failure = FT_New_Face(ft, font_file, 0, &face);
	if (failure) {
		printf("Failed to load font\n");
		FT_Done_FreeType(ft);
		return false;
	}

	// Width calculated dynamically
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Most textures' dimensions are a multiple of four,
	// but we want to loosen that restriction for the moment.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte i = 0; i < 128; i++) {
		// Calls FT_Render_Glyph after font is loaded
		failure = FT_Load_Char(face, i, FT_LOAD_RENDER);
		if (failure != 0) {
			printf("Failed to load glyph: %c", i);
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			return false;
		}

		// TODO: Generate all 128 textures in one call
		GLuint tex;
		glGenTextures(1, &tex); 
		glBindTexture(GL_TEXTURE_2D, tex);

		FT_GlyphSlot glyph = face->glyph;
		Character* c = &out[i];
		c->tex_id = tex;
		c->width = glyph->bitmap.width;
		c->height = glyph->bitmap.rows;
		c->bearing_x = glyph->bitmap_left;
		c->bearing_y = glyph->bitmap_top;
		c->advance = glyph->advance.x;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			c->width,
			c->height,
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

	return true;
}


FontManager* init_font_manager() {
	FontManager* manager = (FontManager*)malloc(sizeof(FontManager));
	manager->font_count = 0;
	return manager;
}


FontSuccess add_font(FontManager* manager, char* filename) {
	FontSuccess out = {};
	out.ok = false;

	FontID id = manager->font_count;
	if (id >= MAX_FONTS) {
		return out;
	}
	Font* font = (Font*)malloc(sizeof(Font));

	bool success = create_font_textures(filename, font->characters);
	if (!success) {
		return out;
	}

	manager->fonts[id] = font;
	manager->font_count++;
	out.ok = true;
	out.id = id;
	return out;
}


void free_font_manager(FontManager* manager) {
	for (int font_i = 0, font_count = manager->font_count; font_i < font_count; font_i++) {
		Font* font = manager->fonts[font_i];
		for (int char_i = 0; char_i < ASCII_CHAR_COUNT; char_i++) {
			// TODO: Delete all textures in a single call
			glDeleteTextures(1, &font->characters[char_i].tex_id);
		}
		free(font);
	}
}