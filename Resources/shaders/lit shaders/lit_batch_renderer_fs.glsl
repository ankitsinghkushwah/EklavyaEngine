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

#define MAX_LIGHTS 100

uniform int n_lights;
uniform Light lights[MAX_LIGHTS];
uniform Material material;



vec4 cal(Light light,vec4 mat_diffuse,vec3 normal)
{
	 
	vec3 light_dir = normalize(light.position.xyz - frag_pos);

	vec4 ambient = light.ambient*mat_diffuse;
	
	//diffuse
	float diff = max(dot(normal,light_dir),0.0f);
	vec4 diffuse = (diff*light.diffuse*mat_diffuse)*light.attenuation;

	return (ambient+diffuse);
}



void main()
{
	
	vec3 normal = -(texture(material.normal_map,tex_coords).rgb);
	normal = normalize(normal * 2.0 - 1.0);


	vec4 mat_diffuse = texture(material.diffuse_map,tex_coords);

	vec4 result = .1f*mat_diffuse;

	for(int i=0; i<n_lights; i++)
		result += cal(lights[i],mat_diffuse,normal);

	final_color = result;
}