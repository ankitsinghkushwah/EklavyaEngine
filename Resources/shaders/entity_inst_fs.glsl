
#version 330

//outs
layout(location = 0) out vec4 finalColor;

//ins
in vec2 texCoords;
in vec2 texIndexVec;

uniform sampler2D samplers[10]; //REMEMBER TO CHANGE THIS IF MORE GEMS ARE ADDED


void main()
{
    int index = int(texIndexVec.x);
    finalColor = texture(samplers[index],texCoords);
}
