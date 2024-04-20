#version 330

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;

void main()
{

	gl_Position = projection*view*model*vec4(pos,0.0,1.0);
	texCoords = tex;

}
