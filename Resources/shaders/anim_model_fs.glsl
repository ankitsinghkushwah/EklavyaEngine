#version 410

out vec4 finalColor;

in vec2  texCoords;

struct Material
{
  sampler2D albedoMap;
};

uniform Material material;

void main()
{
  vec3 color = texture(material.albedoMap, texCoords).rgb;
  finalColor = vec4(color, 1.0f);
}
