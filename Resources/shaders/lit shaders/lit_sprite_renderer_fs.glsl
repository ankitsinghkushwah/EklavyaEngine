#version 430 core


//outs
layout(location = 0) out vec4 final_color;


//ins

in vec2 tex_coords;
in vec3 frag_pos;


struct Light
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	float attenuation;
};


struct Material
{
	sampler2D diffuse_map;
	sampler2D normal_map;
	
	float shininess;
};


uniform Light light;
uniform Material material;

void main()
{
	 vec3 normal = vec3(0.0f,0.0f,1.0f);
	 
	 vec4 mat_diffuse = texture(material.diffuse_map,tex_coords);

	vec3 light_dir = normalize(light.position.xyz - frag_pos);

	vec4 ambient = light.ambient*mat_diffuse;
	
	//diffuse
	float diff = max(dot(normal,light_dir),0.0f);
	vec4 diffuse = (diff*light.diffuse*mat_diffuse)*light.attenuation;

	final_color = ambient+diffuse;
}