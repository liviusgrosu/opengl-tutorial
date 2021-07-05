#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "GLWindow.h"

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
GLWindow mainWindow;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

float angleOffset = 0.0f;
float angleMaxOffset = 360.0f;
float angleIncrement = 0.005f;

bool scaleDirection = true;
float scaleOffset = 0.0f;
float scaleMaxOffset = 1.0f;
float scaleIncrement = 0.0001f;

// Vertex Shader
static const char* vShader = "shaders/shader.vert";

// Fragment Shader
static const char* fShader = "shaders/shader.frag";

void CreateObjects() {

    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj2);
}

void CreateShaders() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);
}

int main() {

    mainWindow = GLWindow(800, 600);
    mainWindow.Initialize();

    CreateObjects();
    CreateShaders();

    GLuint uniformProjection = 0, uniformModel = 0;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.GetBufferWidth() / (GLfloat)mainWindow.GetBufferHeight(), 0.1f, 100.0f);

    while (!mainWindow.GetShouldClose()) {
        glfwPollEvents();

        // Translation offset
        triOffset += direction ? triIncrement : -triIncrement;
        if (abs(triOffset) >= triMaxOffset) {
            direction = !direction;
        }

        // Rotation offset
        angleOffset += angleIncrement;
        if (angleOffset > angleMaxOffset) {
            angleOffset = 0.0f;
        }

        // Scaling offset
        scaleOffset += scaleDirection ? scaleIncrement : -scaleIncrement;
        if (abs(scaleOffset) > scaleMaxOffset) {
            scaleDirection = !scaleDirection;
        }
        
        // Clear the colour
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // Clear the depth buffer so it doesnt use old values
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Assign shader
        shaderList[0]->UseShader();
        {
            uniformModel = shaderList[0]->GetModelLocation();
            uniformProjection = shaderList[0]->GetProjectionLocation();
            // Render 1st mesh
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f, triOffset, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

            meshList[0]->RenderMesh();

            // Render 2nd mesh
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f, -triOffset, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            meshList[1]->RenderMesh();
        }
        // Clear shaders
        glUseProgram(0);

        mainWindow.SwapBuffers();
    }

    return 0;
}