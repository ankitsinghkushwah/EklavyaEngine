#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 weights;
layout(location = 4) in vec4 boneIds;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int    MAX_JOINTS = 100;
const int    MAX_WEIGHTS = 4;
uniform mat4 gBones[MAX_JOINTS];

out vec2     texCoords;

void         main()
{
  vec4 totalPosition = vec4(0.0f);

  for (int i = 0; i < MAX_WEIGHTS; i++)
    {
        int boneID = int(boneIds[i]);
        
        if(boneID < 0) continue;
        
        if(boneID >= MAX_JOINTS)
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = gBones[boneID] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];

    }

    mat4 viewModel = view;
  gl_Position = projection * viewModel * totalPosition;
  texCoords = uv;
}
