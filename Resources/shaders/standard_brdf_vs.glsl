#version 410
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec4 weights;
layout(location = 4) in vec4 boneIds;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;
const int    MAX_JOINTS = 100;
const int    MAX_WEIGHTS = 4;
uniform mat4 gBones[MAX_JOINTS];
uniform bool bApplyAnimation = false;
out          FragParams
{
  mat4 VM;
  vec2 texCoords;
  vec3 VMPos;
  vec3 pos;
  vec3 VMNormal;
  vec3 normal;
  vec4 lightSpacePos;
}
fragParams;
void main()
{
  vec4 totalPosition = vec4(0.0f);
  vec3 totalNormal = vec3(0.0f);
  if (bApplyAnimation)
    {
      for (int i = 0; i < MAX_WEIGHTS; i++)
        {
          int boneID = int(boneIds[i]);
          if (boneID < 0)
            continue;
          if (boneID >= MAX_JOINTS)
            {
              totalPosition = vec4(pos, 1.0f);
              totalNormal = norm;
              break;
            }
          vec4 localPosition = gBones[boneID] * vec4(pos, 1.0f);
          totalPosition += localPosition * weights[i];
          vec3 localNormal = mat3(gBones[boneID]) * norm;
          totalNormal += localNormal * weights[i];
        }
    }
  else
    {
      totalPosition = vec4(pos, 1.0f);
      totalNormal = norm;
    }
  totalNormal = normalize(totalNormal);
  mat4 viewModel = view * model;
  fragParams.VM = viewModel;
  gl_Position = projection * viewModel * totalPosition;
  fragParams.VMPos = vec3(viewModel * totalPosition);
  fragParams.normal = normalize(norm);
  fragParams.pos = vec3(model * totalPosition);
  fragParams.VMNormal = normalize(mat3(viewModel) * totalNormal);
  fragParams.texCoords = tex;
  fragParams.lightSpacePos = lightSpaceMatrix * vec4(fragParams.pos, 1.0f);
}
