#version 330 core

out vec4 out_color;

in FragParams
{
    vec2 texCoords;
    vec3 VMPos;
    vec3 VMNormal;
    vec3 normal;
}fragParams;

struct Material
{
   vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specPow;
};

struct Light
{
	vec3 position;
    vec3 color;
};

uniform Light light;
uniform Material material;

void main()
{
    vec3 normal = fragParams.VMNormal;
    
    vec3 ambient,diffuse;
    ambient = light.color * material.ambient;
    
    vec3 light_dir = normalize(light.position - fragParams.VMPos);
    diffuse = dot(normal,light_dir) * light.color * material.diffuse;
    
    vec3 viewDir = normalize(-fragParams.VMPos);
    vec3 reflectDir = reflect(-light_dir, normal);
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = material.specPow * spec * light.color;

    
    out_color = vec4(ambient + diffuse + specular,1.0);
}
