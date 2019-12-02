#include "fonts.h"
#include "glad/glad.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Fonts {
	struct Node {
		int x;
		int y;
		int w;
		int h;
		char ascii;
		Node* children[2];
	};

	struct FontPacking {
		Node* root;
		int size;
	};

	struct CharacterBuffer {
		// Byte offset to the next line
		int stride;
		unsigned char* data;
	};

	struct ConstructInfo {
		Font* font;
		CharacterBuffer buffers[ASCII_CHAR_COUNT];
	};

	ConstructInfo* create_font_construction_info(Memory::Storage storage, Font* font) {
		ConstructInfo* info = gb_alloc_item(storage.scratch, ConstructInfo);
		info->font = font;
		return info;
	}

	ConstructInfo* create_font_textures(Memory::Storage storage, char* font_file, int point_size) {
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
		FT_Set_Pixel_Sizes(face, 0, point_size);

		Font* font = gb_alloc_item(storage.permanent, Font);
		ConstructInfo* info = create_font_construction_info(storage, font);
		for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
			char ascii = i + ASCII_START;
			failure = FT_Load_Char(face, ascii, FT_LOAD_RENDER);
			if (failure != 0) {
				printf("Failed to load glyph: %c", ascii);
				FT_Done_Face(face);
				FT_Done_FreeType(ft);
				return nullptr;
			}

			FT_GlyphSlot glyph = face->glyph;
			FT_Bitmap bitmap = glyph->bitmap;
			Character* c = &info->font->characters[i];
			CharacterBuffer* buffer = &info->buffers[i];

			c->w = bitmap.width;
			c->h = bitmap.rows;
			c->bearing_x = glyph->bitmap_left;
			c->bearing_y = glyph->bitmap_top;
			c->advance = glyph->advance.x / 64;
			buffer->stride = bitmap.pitch;

			int buffer_bytes = bitmap.pitch * bitmap.rows;
			buffer->data = gb_alloc_array(storage.scratch, unsigned char, buffer_bytes);
			memcpy(buffer->data, bitmap.buffer, buffer_bytes);
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		return info;
	}


	Node* alloc_and_init_node(Memory::Storage storage, int w, int h, int x, int y) {
		Node* node = gb_alloc_item(storage.scratch, Node);
		node->x = x;
		node->y = y;
		node->w = w;
		node->h = h;
		node->ascii = -1;
		node->children[0] = nullptr;
		node->children[1] = nullptr;
		return node;
	}

	bool insert_recursive(Memory::Storage storage, Node* node, Character* c, char ascii) {
		bool is_leaf = node->ascii < 0;
		if (is_leaf) {
			bool fits = c->w < node->w && c->h < node->h;
			if (!fits) {
				return false;
			} 

			unsigned int h_split_r_area = (node->w - c->w) * node->h;
			unsigned int v_split_r_area = node->w * (node->h - c->h);

			// Pick the split direction based on which yields greater area
			// for the second child. 
			Node* left;
			Node* right;
			if (h_split_r_area > v_split_r_area) {
				// Horizontal split
				{
					int w = c->w;
					int h = node->h - c->h;
					int x = node->x;
					int y = node->y + c->h;
					left = alloc_and_init_node(storage, w, h, x, y);
				}
				{
					int w = node->w - c->w;
					int h = node->h;
					int x = node->x + c->w;
					int y = node->y;
					right = alloc_and_init_node(storage, w, h, x, y);
				}
			} else {
				// Vertical split
				{
					int w = node->w - c->w;
					int h = c->h;
					int x = node->x + c->w;
					int y = node->y;
					left = alloc_and_init_node(storage, w, h, x, y);
				}
				{
					int w = node->w;
					int h = node->h - c->h;
					int x = node->x;
					int y = node->y + c->h;
					right = alloc_and_init_node(storage, w, h, x, y);
				}
			}

			node->children[0] = left;
			node->children[1] = right;
			node->w = c->w;
			node->h = c->h;
			node->ascii = ascii;

			return true;
		} else {
			return insert_recursive(storage, node->children[0], c, ascii) ||
			   	insert_recursive(storage, node->children[1], c, ascii);
		}
	}

	FontPacking create_packing(Memory::Storage storage, ConstructInfo* info) {
		int atlas_size = 64;
		FontPacking packing;
		packing.root = alloc_and_init_node(storage, atlas_size, atlas_size, 0, 0);

		for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
			Character* c = &info->font->characters[i];
			char ascii = i + ASCII_START;
			if (!insert_recursive(storage, packing.root, c, ascii)) {
				// Grow atlas and retry
				atlas_size *= 2;
				i = 0;
				packing.root = alloc_and_init_node(storage, atlas_size, atlas_size, 0, 0);
			}
		}

		packing.size = atlas_size;
		return packing;
	}

	unsigned char* rasterize_packing(Memory::Storage storage, FontPacking packing, ConstructInfo* info) {
		int dim = packing.size;
		unsigned char* pixels = gb_alloc_array(storage.scratch, unsigned char, dim * dim);

		Node* node_stack[256];
		node_stack[0] = packing.root;
		int current_node = 0;

		int node_n = 0;
		while (current_node > -1) {
			Node* node = node_stack[current_node];
			current_node--;
			node_n++;

			char ascii = node->ascii;
			if (ascii > -1) {
				int char_idx = ascii - ASCII_START;
				CharacterBuffer buffer = info->buffers[char_idx];

				// Paint
				for (int y = 0; y < node->h; y++) {
					unsigned char* src = &buffer.data[buffer.stride * y];
					unsigned char* dst = &pixels[(y + node->y) * dim + node->x];
					memcpy(dst, src, node->w);
				}

				// Push children onto stack
				for (int i = 0; i < 2; i++) {
					Node* child = node->children[i];
					if (child) {
						current_node++;
						node_stack[current_node] = child;
					}
				}
			}
		}

		return pixels;
	}

	GLuint publish_to_gpu(unsigned char* pixels, int dim) {
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			dim,
			dim,
			0, 
			GL_RED,
			GL_UNSIGNED_BYTE,
			pixels
		);

		return tex;
	}

	Font* from_file(Memory::Storage storage, char* font_file, int point_size) {
		ConstructInfo* info = create_font_textures(storage, font_file, point_size);
		FontPacking packing = create_packing(storage, info);
		unsigned char* pixels = rasterize_packing(storage, packing, info);
		GLuint tex = publish_to_gpu(pixels, packing.size);

		Font* font = info->font;
		font->tex = tex;
		gb_free_all(storage.scratch);
		return font;
	}
}
