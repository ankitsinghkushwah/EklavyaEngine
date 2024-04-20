#version 410 

layout(location = 0) in vec3 pos;

out vec3 sampleDir;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    vec4 finalPos = projection * mat4(mat3(view)) * model * vec4(pos,1.0f);
    gl_Position = finalPos.xyww;
    sampleDir = pos;
}
