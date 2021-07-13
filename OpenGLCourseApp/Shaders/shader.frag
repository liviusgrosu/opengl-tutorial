#version 330

in vec4 vColour;
in vec2 texCoord;

out vec4 colour;

struct DirectionalLight {
	vec3 colour;
	float ambientIntensity;
};

// This is taking from the active texture buffer (ex: GL_TEXTURE0)
uniform sampler2D textureSampler;
uniform DirectionalLight directionalLight;

void main()
{
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;
	colour = texture(textureSampler, texCoord) * ambientColour;
}