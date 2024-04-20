#version 410

out vec4 FragColor;

in FragParams
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

struct Material
{
    int tiling;
    float opacity;
    vec3 color;
    float metallic;
    float roughness;

    int bAlbedo;
    sampler2D albedoMap;
    int bNormal;
    sampler2D normalMap;
    int bMetallic;
    sampler2D metallicMap;
    int bRoughness;
    sampler2D roughnessMap;
    int bAO;
    sampler2D aoMap;
};

uniform mat4 view;

struct Light
{
    vec3 position;
    vec3 color;
};

uniform Light light;
uniform Material material;

const float PI = 3.14159265359;

uniform int bCubemap;
uniform samplerCube cubemap;

uniform sampler2D shadowPassOutput;

//is this fragment in shadow? and apply PCF
float GetShadowValue(float cosTheta)
{
    vec4 sampleCoords = fragParams.lightSpacePos * 0.5 + 0.5;
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowPassOutput, 0);

    if(fragParams.lightSpacePos.z > 1.0f)
        return 0.0f;

    for(int x = -3; x <= 3; ++x)
    {
        for(int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(shadowPassOutput, sampleCoords.xy + vec2(x, y) * texelSize).r;
            shadow += sampleCoords.z < pcfDepth + bias ? 0.0 : 1.0;
        }
    }
    return shadow / 49;
}


vec3 getNormalFromMap()
{
    if(material.bNormal == 1)
    {
        vec2 tex = fragParams.texCoords * material.tiling;
        vec3 tangentNormal = texture(material.normalMap, tex).xyz * 2.0 - 1.0;
        vec3 pos = fragParams.pos;
        vec3 norm = fragParams.normal;
        vec3 Q1 = dFdx(pos);
        vec3 Q2 = dFdy(pos);
        vec2 st1 = dFdx(tex);
        vec2 st2 = dFdy(tex);
        vec3 N = normalize(norm);
        vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
        vec3 B = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);
        return normalize(mat3(fragParams.VM) * (TBN * tangentNormal));
    }
    else
    {
        return fragParams.VMNormal;
    }
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}


float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec2 tex = fragParams.texCoords * material.tiling;

    vec3 albedo;
    float metallic, roughness, ao;

    if(material.bAlbedo == 1)
        albedo = pow(texture(material.albedoMap, tex).rgb, vec3(2.2));
    else
        albedo = material.color;

    if(material.bMetallic == 1)
        metallic = texture(material.metallicMap, tex).r;
    else
        metallic = material.metallic;

    if(material.bRoughness == 1)
        roughness = texture(material.roughnessMap, tex).r;
    else
        roughness = material.roughness;

    if(material.bAO == 1)
        ao = texture(material.aoMap, tex).r;
    else
        ao = 1.0;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(-fragParams.VMPos);

    //    vec3 refl = reflect(-V, fragParams.VMNormal);
    //    refl = normalize(inverse(mat3(view)) * refl);
    //
    //    if(bCubemap == 1)
    //    {
    //        float r = texture(cubemap, refl).r;
    //        albedo = albedo * r;
    //    }

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // calculate per-light radiance
    vec3 L = normalize(light.position - fragParams.VMPos);
    vec3 H = normalize(V + L);
    float distance = length(light.position - fragParams.VMPos);
    float attenuation = 1.0 / (distance * distance);

    float NdotL = max(dot(N, L), 0.0);

    vec3 radiance = light.color;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;


    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float shadow = GetShadowValue(NdotL);
    Lo += ((kD * albedo / PI + specular) * radiance * NdotL) * (1.0f - shadow);

    vec3 ambient = vec3(0.3) * albedo * max(ao, 0.1);

    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0f);
}
