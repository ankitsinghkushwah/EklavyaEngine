#version 400

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec2 offset;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


out vec2 tex_coords;
out vec3 frag_pos;


void main()
{

	tex_coords = tex;
	vec4 position = vec4(pos,0.0f,1.0f);

	position = model*position; //scaling first
	position.xy += offset; //then translating

	frag_pos = position.xyz;
	
	gl_Position = projection*view*position;

}