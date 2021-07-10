#include "Shader.h"
#include <GL/glew.h>

Shader::Shader() {
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation) {
    std::string vertexString = ReadFile(vertexLocation);
    std::string fragmentString = ReadFile(fragmentLocation);

    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation) {
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read '%s' file location...", fileLocation);
        return "";
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

GLuint Shader::GetProjectionLocation() {
    return uniformProjection;
}
GLuint Shader::GetModelLocation() {
    return uniformModel;
}

void Shader::UseShader() {
    // Install the shader
    glUseProgram(shaderID);
}

void Shader::ClearShader() {
    if (shaderID = 0) {
        glDeleteProgram(shaderID);
    }

    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode) {
    // This object stores the shader objects code
    shaderID = glCreateProgram();

    if (!shaderID) {
        printf("Error creating shader program...\n");
        return;
    }

    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar errorLog[1024] = { 0 };

    // Create the exe of the shaders on GPU
    glLinkProgram(shaderID);
    //  Get linking result log
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
        printf("Error linking program: '%s'\n", errorLog);
        return;
    }

    // Validate the program for any errors
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
        printf("Error validating program: '%s'\n", errorLog);
        return;
    }

    // Get the uniforms location from the shader
    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection");
}

void Shader::AddShader(GLuint program, const char* shaderCode, GLenum shaderType) {
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

Shader::~Shader() {
    ClearShader();
}