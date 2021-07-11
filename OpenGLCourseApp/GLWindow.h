#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWindow
{
public:
    GLWindow();
    GLWindow(GLint windowWidth, GLint windowHeight);

    int Initialize();
    GLfloat GetBufferWidth() { return bufferWidth; }
    GLfloat GetBufferHeight() { return bufferHeight; }

    bool GetShouldClose() { return glfwWindowShouldClose(mainWindow); }
    bool* GetKeys() { return keys; }
    GLfloat GetXChange();
    GLfloat GetYChange();

    void SwapBuffers() { glfwSwapBuffers(mainWindow); }

    ~GLWindow();

private:
    GLFWwindow* mainWindow;
    GLint width, height;
    GLint bufferWidth, bufferHeight;

    // range of ascii characters
    bool keys[1024];

    GLfloat lastX;
    GLfloat lastY;
    GLfloat xChange;
    GLfloat yChange;
    bool mouseFirstMoved;

    void CreateCallbacks();
    static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
    static void HandleMouse(GLFWwindow* window, double xPos, double yPos);
};

