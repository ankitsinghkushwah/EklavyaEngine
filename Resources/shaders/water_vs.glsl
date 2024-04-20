#version 330 

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

out vec4 clipSpace;
out vec2 texCoords;
out vec3 fragPos;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
	vec4 worldCoord = model*vec4(inPosition,0.0f,1.0f);
	vec4 clipCoord = projection * view * worldCoord;
	gl_Position = clipCoord;
	clipSpace = clipCoord;
	texCoords = inTexCoords * 6.0f;
	fragPos = worldCoord.xyz;
}
