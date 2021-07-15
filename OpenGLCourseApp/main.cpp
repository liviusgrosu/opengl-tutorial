#define STB_IMAGE_IMPLEMENTATION

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
#include "Camera.h"
#include "Texture.h"
#include "Light.h"

const float toRadians = 3.14159265f / 180.0f;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
GLWindow mainWindow;
Camera mainCamera;

Texture brickTexture;
Texture dirtTexture;

Light mainLight;

GLfloat deltaTime = .0f;
GLfloat lastTime = .0f;


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
        //  x       y       z       s       t
            -1.0f,  -1.0f,  0.0f,   0.0f,   0.0f,
            0.0f,   -1.0f,  1.0f,   0.5f,   0.0f,
            1.0f,   -1.0f,  0.0f,   1.0f,   0.0f,
            0.0f,   1.0f,   0.0f,   0.5f,   1.0f,
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 20, 12);
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

    mainCamera = Camera(glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, 1.0f, .0f), -90.0f, .0f, 5.0f, 0.1f);

    brickTexture = Texture((char*)"Textures/brick.png");
    brickTexture.LoadTexture();
    dirtTexture = Texture((char*)"Textures/dirt.png");
    dirtTexture.LoadTexture();

    mainLight = Light(1.0f, 1.0f, 1.0f, 0.5f);
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.GetBufferWidth() / (GLfloat)mainWindow.GetBufferHeight(), 0.1f, 100.0f);

    while (!mainWindow.GetShouldClose()) {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        // Handle window resizing, moving, menu operations, etc...
        glfwPollEvents();
        mainCamera.KeyControl(mainWindow.GetKeys(), deltaTime);
        mainCamera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());

        // Clear the colour
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // Clear the depth buffer so it doesnt use old values
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Assign shader to the rendering states
        shaderList[0]->UseShader();
        {
            mainLight.UseLight(shaderList[0]->GetAmbientIntensityLocation(), shaderList[0]->GetAmbientColourLocation());
            glm::mat4 model(1.0f);

            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            // Pass the model and projection to the shaders
            glUniformMatrix4fv(shaderList[0]->GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(shaderList[0]->GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(shaderList[0]->GetViewLocation(), 1, GL_FALSE, glm::value_ptr(mainCamera.CalculateViewMatrix()));
            
            brickTexture.UseTexture();
            // Draw the mesh
            meshList[0]->RenderMesh();

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(shaderList[0]->GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
            dirtTexture.UseTexture();
            meshList[1]->RenderMesh();
        }
        // Clear shaders
        glUseProgram(0);

        mainWindow.SwapBuffers();
    }

    return 0;
}