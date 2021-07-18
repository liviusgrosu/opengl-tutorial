#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

struct Material
{
	float specularIntensity;
	float shininess;
};

struct DirectionalLight 
{
	vec3 colour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform vec3 cameraPosition;

void main()
{
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
	vec4 diffuseColour = vec4(directionalLight.colour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0, 0, 0, 0);

	if (diffuseFactor > 0.0f)
	{
		vec3 fragmentToCamera = normalize(cameraPosition - FragPos);
		vec3 reflectedLightRay = normalize(reflect(directionalLight.direction, normalize(Normal)));
	 
		float specularFactor = dot(fragmentToCamera, reflectedLightRay);
		if (specularFactor > 0.0f) 
		{
			// The larger the shininess is the smaller the spot will be
			specularFactor = pow(specularFactor, material.shininess);
			specularColour = vec4(directionalLight.colour * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	colour = texture(theTexture, TexCoord) * (ambientColour + diffuseColour + specularColour);
}