#version 330

layout(location = 0) out vec4 finalColor;

in vec2 texCoords;

struct Material
{
	sampler2D albedoMap;
	bool bApplyColor;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specPow;
};

uniform Material material;

void main()
{
	if(material.bApplyColor)
		finalColor = vec4(material.diffuse,1.0f);
	else
	    finalColor = texture(material.albedoMap,texCoords);
}
