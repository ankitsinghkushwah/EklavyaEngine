/*
	particle_generator_fs.glsl
*/

#version 330

//outs
layout(location = 0) out vec4 final_color;

//ins
in vec2 tex_coords;

uniform vec4 color;
uniform sampler2D sampler0;
uniform bool usecolor;

uniform float inner_radius = 0.46f;
uniform float outer_radius = 0.47f;

void main()
{
	vec4 alpha = vec4(0.0f,0.0f,0.0f,0.0f);
	float dx = .5f - tex_coords.x;
	float dy = .5f - tex_coords.y;
	float dist = sqrt((dx*dx) + (dy*dy));
	
	if(usecolor)
	final_color = mix(color,alpha,smoothstep(inner_radius,outer_radius,dist));
	else
	{
	final_color = mix(texture(sampler0,tex_coords),alpha,smoothstep(inner_radius,outer_radius,dist));
	}
}
