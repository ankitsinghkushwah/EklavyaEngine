#version 410

out vec4 finalColor;
uniform sampler2D mainOutputPass;
in vec2 texCoords;

float fix_it(float depth)
{
    float near_plane = 0.1f;
    float far_plane = 100.0f;
    float z = depth * 2.0 - 1.0;  // Transform depth from [0, 1] to [-1, 1]
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float r = texture(mainOutputPass,texCoords).r;
    float fixed_r = fix_it(r);
    finalColor = vec4(vec3(r),1.0f);
//    finalColor = texture(mainOutputPass,texCoords);
}
