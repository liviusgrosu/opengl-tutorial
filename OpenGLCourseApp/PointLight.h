#pragma once
#include "Light.h"
#include "OmniShadowMap.h"

class PointLight : public Light
{
public:
    PointLight();

    PointLight(GLfloat shadowWidth, GLfloat shadowHeight,
                GLfloat nearPlane, GLfloat farPlane,
                GLfloat red, GLfloat green, GLfloat blue, 
                GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                GLfloat xPosition, GLfloat yPosition, GLfloat zPosition, 
                GLfloat constant, GLfloat linear, GLfloat exponent);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                    GLuint diffuseIntensityLocation, GLuint positionLocation, 
                    GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

    std::vector<glm::mat4> CalculateLightTransform();
    GLfloat GetFarPlane();
    glm::vec3 GetPosition();

    ~PointLight();

protected:
    glm::vec3 position;

    GLfloat constant, linear, exponent;

    GLfloat farPlane;
};

