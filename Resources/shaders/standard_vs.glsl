#version 330 

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;


uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 PV;
uniform mat4 PMV;

out FragParams
{
	vec2 texCoords;
	vec3 VMPos;
	vec3 VMNormal;
    vec3 R;
}fragParams;

void main()
{	
	mat4 viewModel = view * model;
    gl_Position =  projection * view * model * vec4(pos,1.0f);
	vec3 viewSpacePos = vec3(viewModel * vec4(pos,1.0f));
    vec3 viewSpaceNormal = normalize(mat3(viewModel) * norm);
    fragParams.VMNormal =viewSpaceNormal;
    fragParams.VMPos = viewSpacePos;
    
    vec3 V = normalize(-viewSpacePos);
    vec3 Ref = reflect(-V,viewSpaceNormal);
    fragParams.R = vec3(viewModel * vec4(Ref,0.0f));
    
	fragParams.texCoords = tex;
}
