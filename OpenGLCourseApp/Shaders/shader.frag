#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

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

struct OmniShadowMap
{
	samplerCube shadowMap;
	float farPlane;
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

// Texture unit 0
uniform sampler2D theTexture; 
// Texture unit 1
uniform sampler2D directionalShadowMap;

uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform Material material;	
uniform vec3 cameraPosition;

float CalculateDirectionalShadowFactor(DirectionalLight light)
{
	// This will convert the coordinates to a range between -1 and 1
	vec3 projectionCoordinates = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
	// Convert that range to 0 and 1
	projectionCoordinates = (projectionCoordinates * 0.5) + 0.5;

	float currentDepth = projectionCoordinates.z;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.direction);

	// Eliminate shadow acne
	float bias = max(0.05 * (1- dot(normal, lightDirection)), 0.005);

	// 1.0f - full shadow
	// 0.0f - no shadow
	// Check if the current point is behind or in front of the closest point
	float shadow = 0.0;

	// Get how big one texel size since shadow and texture sizes are different
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);

	for (int x = -3; x <= 3; ++x)
	{
		for (int y = -3; y <= 3; ++y)
		{
			// This is also are closest value in the depth/shadow map
			float pcfDepth = texture(directionalShadowMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	// 7x7 texel grid for PCF
	shadow /= 49.0;

	// Any shadow away from the frustum will be lit
	if(projectionCoordinates.z > 1.0) 
	{
		shadow = 0;
	}

	return shadow;
}

float CalculateOmniShadowFactor(PointLight light, int shadowIndex)
{
	vec3 fragmentToLight = FragPos - light.position;
	float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragmentToLight).r;

	closest *= omniShadowMaps[shadowIndex].farPlane;

	float currentDepth = length(fragmentToLight);
	float bias = 0.05f;
	float shadow = currentDepth - bias > closest ? 1.0 : 0.0;

	return shadow;
}

vec4 CalculateLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	// Static lighting 
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;
	
	// Get the angle between direction to light and point normal
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColour = vec4(light.colour, 1.0f) * light.diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0, 0, 0, 0);

	// If the light direction can hit the point...
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

	return ambientColour + (1.0f - shadowFactor) * (diffuseColour + specularColour);
}

vec4 CalculateDirectionalLight()
{
	float shadowFactor = CalculateDirectionalShadowFactor(directionalLight);
	return CalculateLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalculatePointLight(PointLight pointLight, int shadowIndex)
{
	vec3 direction = FragPos - pointLight.position;
	float distance = length(direction);
	direction = normalize(direction);

	float shadowFactor = CalculateOmniShadowFactor(pointLight, shadowIndex);
	
	vec4 colour = CalculateLightByDirection(pointLight.base, direction, shadowFactor);
	float attenuation = pointLight.exponent * distance * distance +
						pointLight.linear * distance +
						pointLight.constant;
	
	return colour / attenuation;
}

vec4 CalculateSpotLight(SpotLight spotLight, int shadowIndex)
{
	vec3 rayDirection = normalize(FragPos - spotLight.base.position);
	float spotlightFactor = dot(rayDirection, spotLight.direction);
	
	if (spotlightFactor > spotLight.edge)
	{
		// Only calculate light if it's within a range
		vec4 colour = CalculatePointLight(spotLight.base, shadowIndex);
		// Calculate the edge diminishing effect
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
		totalColour += CalculatePointLight(pointLights[i], i);
	}
	
	return totalColour;
}

vec4 CalculateSpotLights()
{
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < spotLightCount; i++)
	{
		totalColour += CalculateSpotLight(spotLights[i], i + pointLightCount);
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