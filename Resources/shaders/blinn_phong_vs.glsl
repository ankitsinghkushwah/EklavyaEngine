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

// Lighting is done in view space, so the camera sits at the origin.
out FragParams
{
  vec2 texCoords;
  vec3 viewPos;
  vec3 viewNormal;
  vec4 lightSpacePos;
}
fragParams;

void main()
{
  vec4 localPos = vec4(0.0);
  vec3 localNormal = vec3(0.0);

  if (bApplyAnimation)
  {
    for (int i = 0; i < MAX_WEIGHTS; i++)
    {
      int boneID = int(boneIds[i]);
      if (boneID < 0)
        continue;
      if (boneID >= MAX_JOINTS)
      {
        localPos = vec4(pos, 1.0);
        localNormal = norm;
        break;
      }
      localPos += gBones[boneID] * vec4(pos, 1.0) * weights[i];
      localNormal += mat3(gBones[boneID]) * norm * weights[i];
    }
  }
  else
  {
    localPos = vec4(pos, 1.0);
    localNormal = norm;
  }

  mat4 viewModel = view * model;
  // Inverse-transpose keeps normals perpendicular on non-uniformly scaled meshes (e.g. long boxes).
  mat3 normalMatrix = transpose(inverse(mat3(viewModel)));

  vec4 worldPos = model * localPos;
  vec4 viewPos = view * worldPos;

  fragParams.texCoords = tex;
  fragParams.viewPos = viewPos.xyz;
  fragParams.viewNormal = normalize(normalMatrix * localNormal);
  fragParams.lightSpacePos = lightSpaceMatrix * worldPos;

  gl_Position = projection * viewPos;
}
