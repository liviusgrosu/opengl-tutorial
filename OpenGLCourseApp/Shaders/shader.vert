#version 330

// 0 is the location id of this input variable
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 DirectionalLightSpacePos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 directionalLightTransform;

void main()
{
	// Calculate the position
	gl_Position = projection * view * model * vec4(pos, 1.0);
	// Convert this vertex to the lights coordinates space
	DirectionalLightSpacePos = directionalLightTransform * model * vec4(pos, 1.0f);
	
	TexCoord = tex;
	
	Normal = mat3(transpose(inverse(model))) * norm;

	// Provide the fragment position to the fragment shader
	FragPos = (model * vec4(pos, 1.0)).xyz;
}