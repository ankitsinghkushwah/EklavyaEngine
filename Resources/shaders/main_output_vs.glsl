#version 410

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

uniform mat4 model;
uniform mat4 projection;
out vec2 texCoords;

void main()
{
    gl_Position = vec4(pos,0.0f,1.0f);
    texCoords = tex;
}
