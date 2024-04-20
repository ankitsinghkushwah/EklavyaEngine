#version 330

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec2 scale;
layout(location = 3) in vec2 offset;

uniform mat4 view;
uniform mat4 projection;

uniform int texIndices[150];

out vec2 texCoords;
out vec2 texIndexVec;

void main()
{
    texCoords = tex;
    texIndexVec = vec2(texIndices[gl_InstanceID]);
    vec4 position = vec4(pos,0.0f,1.0f);
    
    position.xy*=scale;
    position.xy += offset; //then translating
    gl_Position = projection*position;
}
