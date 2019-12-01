#version 330 core
out vec4 FragColor;

in vec2 texture_coord;

uniform sampler2D font_texture;

void main()
{
	FragColor = texture(font_texture, texture_coord);
	// FragColor = vec4(texture_coord, 0.0f, 1.0f);
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 
