#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

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
static const char* vShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
// 0 is the location id of this input variable                  \n\
layout(location = 0) in vec3 pos;                               \n\
                                                                \n\
out vec4 vColour;                                               \n\
                                                                \n\
uniform mat4 model;                                             \n\
                                                                \n\
void main()                                                     \n\
{                                                               \n\
    // gl_position is a built in output variable                \n\
    gl_Position = model * vec4(pos, 1.0);                       \n\
    vColour = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);               \n\
}";

// Fragment Shader
static const char* fShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
                                                                \n\
in vec4 vColour;                                                \n\
// Because this is the only ouput, GLSL will assume this is the \n\
// colour it needs                                              \n\
out vec4 colour;                                                \n\
                                                                \n\
void main()                                                     \n\
{                                                               \n\
    colour = vColour;                                           \n\
}                                                               \n\
";

void AddShader(GLuint program, const char* shaderCode, GLenum shaderType) {
    // Create empty shader type
    GLuint mainShader = glCreateShader(shaderType);
    const GLchar* glShaderCode[1];
    glShaderCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(mainShader, 1, glShaderCode, codeLength);
    glCompileShader(mainShader);

    GLint result = 0;
    GLchar errorLog[1024] = { 0 };

    glGetShaderiv(mainShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(mainShader, sizeof(errorLog), NULL, errorLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, errorLog);
        return;
    }

    glAttachShader(program, mainShader);
}

void CompiledShaders() {
    // This object stores the shader objects code
    shader = glCreateProgram();

    if (!shader) {
        printf("Error creating shader program...\n");
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar errorLog[1024] = { 0 };

    // Create the exe of the shaders on GPU
    glLinkProgram(shader);
    //  Get linking result log
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
        printf("Error linking program: '%s'\n", errorLog);
        return;
    }

    // Validate the program for any errors
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
        printf("Error validating program: '%s'\n", errorLog);
        return;
    }

    // Get the uniform from the shader
    uniformModel = glGetUniformLocation(shader, "model");
}

void CreateTriangle() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // Generate that vertex array and store the id
    glGenVertexArrays(1, &VAO);
    // Bind the vertex array with that ID
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
            
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // How to read the vertices data
    // This 0 is refering to the 0 location id in the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // This attaches to a buffer in the GPU
    // The shader can then pick this up
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

int main() {
    if (!glfwInit()) {
        // GLFW init failed
        printf("GLFW init failed...");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Don't want to use open depricated functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW window failed to create...");
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extensions features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("GLEW init failed...");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateTriangle();
    CompiledShaders();
    
    while (!glfwWindowShouldClose(mainWindow)) {
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
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Assign shader
        glUseProgram(shader);
            glm::mat4 model(1.0f);
            
            // model = glm::scale(model, glm::vec3(scaleOffset, scaleOffset, 1.0f));
            // model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
            // model = glm::rotate(model, angleOffset * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));        

            glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);

        // Clear shaders
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}