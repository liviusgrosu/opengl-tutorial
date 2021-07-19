#pragma once
#include "Light.h"

class PointLight : Light
{
public:
    PointLight();

    PointLight(GLfloat red, GLfloat green, GLfloat blue, 
                GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                GLfloat xPosition, GLfloat yPosition, GLfloat zPosition, 
                GLfloat constant, GLfloat linear, GLfloat expontent);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                    GLuint diffuseIntensityLocation, GLuint positionLocation, 
                    GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

    ~PointLight();

private:
    glm::vec3 position;

    GLfloat constant, linear, exponent;
};

