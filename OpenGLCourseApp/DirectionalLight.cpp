#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light() {
    direction = glm::vec3(0.0f, -1.0f, 0.0f);
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

DirectionalLight::DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight, 
                                    GLfloat red, GLfloat green, GLfloat blue, 
                                    GLfloat ambientIntensity, GLfloat diffuseIntensity,
                                    GLfloat xDirection, GLfloat yDirection, GLfloat zDirection) : 
                                    Light(shadowWidth, shadowHeight, red, green, blue, ambientIntensity, diffuseIntensity) {
    // Set the direction and projection of the light
    direction = glm::vec3(xDirection, yDirection, zDirection);
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

void DirectionalLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                                GLuint diffuseIntensityLocation, GLuint directionLocation) {

    glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);

    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
    // Provide a estimated position of the light
    return lightProjection * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

DirectionalLight::~DirectionalLight() {

}