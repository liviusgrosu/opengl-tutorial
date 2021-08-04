#include "PointLight.h"

PointLight::PointLight() : Light() {
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	constant = 1.0f;
	linear = 0.0f;
	exponent = 0.0f;
}

PointLight::PointLight(GLfloat shadowWidth, GLfloat shadowHeight,
						GLfloat nearPlane, GLfloat farPlane, 
						GLfloat red, GLfloat green, GLfloat blue,
						GLfloat ambientIntensity, GLfloat diffuseIntensity,
						GLfloat xPosition, GLfloat yPosition, GLfloat zPosition,
						GLfloat constant, GLfloat linear, GLfloat exponent) : 
						Light(shadowWidth, shadowHeight, red, green, blue, ambientIntensity, diffuseIntensity) {
	// Set the position and constants for the ligh
	position = glm::vec3(xPosition, yPosition, zPosition);
	this->constant = constant;
	this->linear = linear;
	this->exponent = exponent;

	this->farPlane = farPlane;

	float aspect = (float)shadowWidth / (float)shadowHeight;
	lightProjection = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);

	shadowMap = new OmniShadowMap();
	shadowMap->Initialize(shadowWidth, shadowHeight);
}

void PointLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
							GLuint diffuseIntensityLocation, GLuint positionLocation,
							GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation) {
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);

	glUniform3f(positionLocation, position.x, position.y, position.z);
	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(exponentLocation, exponent);
}

std::vector<glm::mat4> PointLight::CalculateLightTransform()
{
	std::vector<glm::mat4> lightTransforms;
	lightTransforms.push_back(lightProjection *
		glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	lightTransforms.push_back(lightProjection *
		glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	lightTransforms.push_back(lightProjection *
		glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	lightTransforms.push_back(lightProjection *
		glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	lightTransforms.push_back(lightProjection *
		glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	lightTransforms.push_back(lightProjection *
		glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	return lightTransforms;
}

glm::vec3 PointLight::GetPosition()
{
	return position;
}

GLfloat PointLight::GetFarPlane()
{
	return farPlane;
}


PointLight::~PointLight() {}