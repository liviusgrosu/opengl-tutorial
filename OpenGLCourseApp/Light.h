#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

class Light
{
public:
    Light();
    Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, 
            GLfloat xDirection, GLfloat yDirection, GLfloat zDirection, GLfloat diffuseIntensity);
    void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, 
                    GLfloat diffuseIntensityLocation, GLfloat directionLocation);
    ~Light();

private:
    glm::vec3 colour;
    GLfloat ambientIntensity;

    glm::vec3 direction;
    GLfloat diffuseIntensity;
};

