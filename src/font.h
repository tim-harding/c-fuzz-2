#include <stdint.h>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using FontID = uint8_t;
struct FontSuccess {
    FontID id;
    bool ok;
};

struct FontManager;

FontManager* init_font_manager();
FontSuccess add_font(FontManager* manager, char* filename);
void free_font_manager(FontManager* manager);