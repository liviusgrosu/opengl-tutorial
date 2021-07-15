#include "Light.h"

Light::Light() {
    colour = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientIntensity = 1.0f;
}

Light::Light(GLfloat red, GLfloat blue, GLfloat green, GLfloat ambientIntensity) {
    colour = glm::vec3(red, blue, green);
    this->ambientIntensity = ambientIntensity;
}

void Light::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation) {
    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);
}

Light::~Light() {
}