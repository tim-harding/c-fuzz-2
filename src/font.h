#pragma once

const char ASCII_START = 32; // Space
const char ASCII_END = 126;  // ~
const char ASCII_CHAR_COUNT = ASCII_END - ASCII_START;

typedef struct Fonts Fonts;
typedef unsigned int FontHandle;

Fonts* init_fonts();
FontHandle create_font(Fonts* manager, char* font_file, int point_size);
unsigned int tex_handle_for_font(Fonts* manager, FontHandle handle);
