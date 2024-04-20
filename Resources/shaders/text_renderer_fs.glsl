#version 330

//outs
layout(location = 0) out vec4 final_color;

//ins
in vec2 tex_coords;

//Uniforms
uniform vec3 font_color;
uniform float width;
uniform float edge;
uniform bool use_color;

uniform sampler2D texture_atlas;
uniform sampler2D font_texture;



void main()
{
	float dist = 1.0f - texture(texture_atlas,tex_coords).a;
    float w = width;
	float alpha = 1.0f - smoothstep(w,w+edge,dist);
	
	if (use_color)
        final_color = vec4(font_color,alpha);
	else
		final_color = vec4(texture(font_texture, tex_coords).xyz, alpha);
}
