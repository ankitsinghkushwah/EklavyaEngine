#version 410

out vec4 finalColor;

uniform vec3 color;
uniform float opacity;

void main()
{
    vec3 col = color;
    finalColor = vec4(col,opacity);
}
