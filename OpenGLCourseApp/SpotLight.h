#pragma once
#include "PointLight.h"
class SpotLight : public PointLight
{
public:
	SpotLight();
	
	SpotLight(GLuint shadowWidth, GLuint shadowHeight,
				GLfloat nearPlane, GLfloat farPlane,
				GLfloat red, GLfloat green, GLfloat blue,
				GLfloat ambientIntensity, GLfloat diffuseIntensity,
				GLfloat xPosition, GLfloat yPosition, GLfloat zPosition,
				GLfloat xDirection, GLfloat yDirection, GLfloat zDirection,
				GLfloat constant, GLfloat linear, GLfloat exponent,
				GLfloat edge);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
					GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
					GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
					GLuint edgeLocation);
	
	void SetFlash(glm::vec3 position, glm::vec3 direction);
	void Toggle() { isOn = !isOn; }

	~SpotLight();

private:
	glm::vec3 direction;

	GLfloat edge;
	GLfloat processedEdge;

	bool isOn;
};

