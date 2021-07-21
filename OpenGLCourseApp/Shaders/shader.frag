#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform Material material;
uniform vec3 cameraPosition;

vec4 CalculateLightByDirection(Light light, vec3 direction)
{
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColour = vec4(light.colour, 1.0f) * light.diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0, 0, 0, 0);

	if (diffuseFactor > 0.0f)
	{
		vec3 fragmentToCamera = normalize(cameraPosition - FragPos);
		vec3 reflectedLightRay = normalize(reflect(direction, normalize(Normal)));
	 
		float specularFactor = dot(fragmentToCamera, reflectedLightRay);
		if (specularFactor > 0.0f) 
		{
			// The larger the shininess is the smaller the spot will be
			specularFactor = pow(specularFactor, material.shininess);
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	return ambientColour + diffuseColour + specularColour;
}

vec4 CalculateDirectionalLight()
{
	return CalculateLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalculatePointLight(PointLight pointLight)
{
	vec3 direction = FragPos - pointLight.position;
	float distance = length(direction);
	direction = normalize(direction);
	
	vec4 colour = CalculateLightByDirection(pointLight.base, direction);
	float attenuation = pointLight.exponent * distance * distance +
						pointLight.linear * distance +
						pointLight.constant;
	
	return colour / attenuation;
}

vec4 CalculateSpotLight(SpotLight spotLight)
{
	vec3 rayDirection = normalize(FragPos - spotLight.base.position);
	float spotlightFactor = dot(rayDirection, spotLight.direction);
	
	if (spotlightFactor > spotLight.edge)
	{
		vec4 colour = CalculatePointLight(spotLight.base);
		return colour * (1.0f - (1.0f - spotlightFactor) * (1.0f / (1.0f - spotLight.edge)));
	}
	else
	{
		return vec4(0, 0, 0, 0);
	}
}

vec4 CalculatePointLights()
{
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++)
	{
		totalColour += CalculatePointLight(pointLights[i]);
	}
	
	return totalColour;
}

vec4 CalculateSpotLights()
{
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < spotLightCount; i++)
	{
		totalColour += CalculateSpotLight(spotLights[i]);
	}
	
	return totalColour;
}

void main()
{
	vec4 finalColour = CalculateDirectionalLight();
	finalColour += CalculatePointLights();
	finalColour += CalculateSpotLights();

	colour = texture(theTexture, TexCoord) * finalColour;
}