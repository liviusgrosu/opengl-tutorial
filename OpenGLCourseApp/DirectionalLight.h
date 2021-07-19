#pragma once
#include "Light.h"
class DirectionalLight : Light
{
public:
    DirectionalLight();

    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                        GLfloat xDirection, GLfloat yDirection, GLfloat zDirection);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                    GLuint diffuseIntensityLocation, GLuint directionLocation);

    ~DirectionalLight();

private:
    glm::vec3 direction;
};

