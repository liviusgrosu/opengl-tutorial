#include "Light.h"

Light::Light() {
    colour = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientIntensity = 1.0f;
    diffuseIntensity = 0.0f;
}

Light::Light(GLfloat shadowWidth, GLfloat shadowHeight, 
                GLfloat red, GLfloat blue, GLfloat green, 
                GLfloat ambientIntensity, GLfloat diffuseIntensity) {
    shadowMap = new ShadowMap();
    shadowMap->Initialize(shadowWidth, shadowHeight);

    colour = glm::vec3(red, blue, green);
    this->ambientIntensity = ambientIntensity;
    this->diffuseIntensity = diffuseIntensity;
}

Light::~Light()
{
}
