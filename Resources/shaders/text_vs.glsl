#version 430 core


layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 tex;
layout(location = 2) in vec2 offset;
layout(location = 3) in vec2 scale;

//uniforms
uniform mat4 projection;

//outs
out vec2 tex_coords;


void main()
{

	vec2 texCoords[] = vec2[](
		vec2(tex.x,tex.y+tex.w),
		vec2(tex.x+tex.z,tex.y+tex.w),
		vec2(tex.x,tex.y),
		vec2(tex.x+tex.z,tex.y)
	);
	
	tex_coords = texCoords[ gl_VertexID ];

	vec4 position = vec4(pos,0.0f,1.0f);
	position.xy*=scale;
	position.xy+=offset;

	gl_Position = projection*position;
}