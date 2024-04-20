#version 330 

in vec2 texCoords;

uniform sampler2D sampler0;
uniform vec4 color;
uniform bool use_color;

out vec4 finalColor;

void main()
{  
	if (use_color)
		finalColor = color * texture(sampler0, texCoords); 
	else
		finalColor = texture(sampler0, texCoords);  
}
