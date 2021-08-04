#include "SpotLight.h"

SpotLight::SpotLight() : PointLight() {
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	edge = 0;
	processedEdge = cosf(glm::radians(edge));
}

SpotLight::SpotLight(GLuint shadowWidth, GLuint shadowHeight,
	GLfloat nearPlane, GLfloat farPlane,
	GLfloat red, GLfloat green, GLfloat blue,
	GLfloat ambientIntensity, GLfloat diffuseIntensity,
	GLfloat xPosition, GLfloat yPosition, GLfloat zPosition,
	GLfloat xDirection, GLfloat yDirection, GLfloat zDirection,
	GLfloat constant, GLfloat linear, GLfloat exponent,
	GLfloat edge) : PointLight(shadowWidth, shadowHeight, nearPlane, farPlane, red, green, blue, ambientIntensity, diffuseIntensity, xPosition, yPosition, zPosition, constant, linear, exponent) {
	
	direction = glm::normalize(glm::vec3(xDirection, yDirection, zDirection));
	this->edge = edge;
	processedEdge = cosf(glm::radians(edge));
}

void SpotLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
							GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
							GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
							GLuint edgeLocation) {
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);

	glUniform3f(positionLocation, position.x, position.y, position.z);
	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(exponentLocation, exponent);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(edgeLocation, processedEdge);
}

void SpotLight::SetFlash(glm::vec3 position, glm::vec3 direction) {
	this->position = position;
	this->direction = direction;
}

SpotLight::~SpotLight() {

}