#version 330

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 tex;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec4 clipPlane;

out vec2 texCoords;

void main()
{	
	vec4 worldPosition = model*vec4(pos,1.0f);
	gl_ClipDistance[0] = dot(worldPosition,clipPlane);
    gl_Position =  projection * view * model * vec4(pos,1.0f);    
	texCoords = tex;
}
