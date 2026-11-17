#version 410

out vec4 FragColor;

in FragParams
{
  vec2 texCoords;
  vec3 viewPos;
  vec3 viewNormal;
  vec4 lightSpacePos;
}
fragParams;

struct Material
{
  int tiling;
  float opacity;
  vec3 color;              // sRGB, used when there is no albedo map
  float specularStrength;  // 0..1
  float shininess;         // Blinn-Phong exponent

  int bAlbedo;
  sampler2D albedoMap;
  int bNormal;
  sampler2D normalMap;
};

// Directional sun. direction points from the surface towards the sun, in view space.
struct Light
{
  vec3 direction;
  vec3 color;
};

uniform mat4 view;
uniform Light light;
uniform Material material;
uniform sampler2D shadowPassOutput;

// Scene look. All colours are linear.
const vec3  SKY_COLOR = vec3(0.42, 0.52, 0.66);    // ambient from above: soft blue
const vec3  GROUND_COLOR = vec3(0.26, 0.23, 0.20); // ambient from below: warm bounce
const float AMBIENT_STRENGTH = 0.55;
const vec3  FILL_COLOR = vec3(0.30, 0.36, 0.45);   // weak cool light opposite the sun
const float RIM_STRENGTH = 0.18;
const float EXPOSURE = 1.0;

vec3 SrgbToLinear(vec3 c) { return pow(c, vec3(2.2)); }
vec3 LinearToSrgb(vec3 c) { return pow(c, vec3(1.0 / 2.2)); }

// Filmic curve (Narkowicz ACES fit): keeps highlights from clipping and keeps colour saturation.
vec3 ToneMapACES(vec3 x)
{
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// 1 = fully lit, 0 = fully in shadow. 7x7 PCF for soft edges.
float ShadowVisibility()
{
  vec3 coords = fragParams.lightSpacePos.xyz / fragParams.lightSpacePos.w;
  coords = coords * 0.5 + 0.5;

  if (coords.z > 1.0 || any(lessThan(coords.xy, vec2(0.0))) || any(greaterThan(coords.xy, vec2(1.0))))
    return 1.0;

  const float bias = 0.005;
  vec2 texelSize = 1.0 / textureSize(shadowPassOutput, 0);

  float shadow = 0.0;
  for (int x = -3; x <= 3; ++x)
  {
    for (int y = -3; y <= 3; ++y)
    {
      float depth = texture(shadowPassOutput, coords.xy + vec2(x, y) * texelSize).r;
      shadow += coords.z - bias > depth ? 1.0 : 0.0;
    }
  }
  return 1.0 - shadow / 49.0;
}

// Normal mapping without stored tangents: build the tangent frame from screen-space derivatives.
vec3 GetNormal(vec2 uv)
{
  vec3 N = normalize(fragParams.viewNormal);

  if (material.bNormal == 0)
    return N;

  vec3 tangentNormal = texture(material.normalMap, uv).xyz * 2.0 - 1.0;

  vec3 dp1 = dFdx(fragParams.viewPos);
  vec3 dp2 = dFdy(fragParams.viewPos);
  vec2 duv1 = dFdx(uv);
  vec2 duv2 = dFdy(uv);

  vec3 dp2perp = cross(dp2, N);
  vec3 dp1perp = cross(N, dp1);
  vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
  vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
  float invMax = inversesqrt(max(dot(T, T), dot(B, B)));
  mat3 TBN = mat3(T * invMax, B * invMax, N);

  return normalize(TBN * tangentNormal);
}

void main()
{
  vec2 uv = fragParams.texCoords * float(material.tiling);

  vec3 albedo;
  float alpha = material.opacity;
  if (material.bAlbedo == 1)
  {
    vec4 albedoTex = texture(material.albedoMap, uv);
    albedo = SrgbToLinear(albedoTex.rgb);
    alpha *= albedoTex.a;
  }
  else
  {
    albedo = SrgbToLinear(material.color);
  }

  vec3 N = GetNormal(uv);
  vec3 V = normalize(-fragParams.viewPos);
  vec3 L = normalize(light.direction);
  vec3 H = normalize(L + V);
  vec3 up = normalize(mat3(view) * vec3(0.0, 1.0, 0.0));

  float NdotL = max(dot(N, L), 0.0);
  float NdotH = max(dot(N, H), 0.0);
  float NdotV = max(dot(N, V), 0.0);

  // Sun: Lambert diffuse plus normalised Blinn-Phong specular, both shadowed.
  // (shininess + 8) / 8 keeps the highlight's total energy roughly constant as it gets sharper.
  float visibility = ShadowVisibility();
  float specNorm = (material.shininess + 8.0) / 8.0;
  float spec = material.specularStrength * specNorm * pow(NdotH, material.shininess);
  vec3 diffuseWeight = albedo * (1.0 - material.specularStrength);
  vec3 sun = (diffuseWeight + vec3(spec)) * light.color * NdotL * visibility;

  // Hemisphere ambient: sky colour from above, ground bounce from below.
  float hemi = dot(N, up) * 0.5 + 0.5;
  vec3 ambient = mix(GROUND_COLOR, SKY_COLOR, hemi) * AMBIENT_STRENGTH * albedo;

  // Fill light from the opposite side of the sun, so shadowed sides keep their shape.
  vec3 sunHorizontal = L - up * dot(L, up);
  vec3 fillDir = normalize(-sunHorizontal + up * 0.5);
  float fillNdotL = max(dot(N, fillDir), 0.0);
  vec3 fill = FILL_COLOR * fillNdotL * albedo;

  // Rim light: a soft sky-coloured edge that separates objects from the background.
  float rim = pow(1.0 - NdotV, 3.0) * RIM_STRENGTH * hemi;
  vec3 rimLight = SKY_COLOR * rim;

  vec3 color = sun + ambient + fill + rimLight;

  color = ToneMapACES(color * EXPOSURE);
  color = LinearToSrgb(color);

  FragColor = vec4(color, alpha);
}
