#include "GLWindow.h"
GLWindow::GLWindow() {
    width = 800;
    height = 600;
    
    xChange = 0.0f;
    yChange = 0.0f;

    for (size_t i = 0; i < 1024; i++) {
        keys[i] = 0;
    }
}
GLWindow::GLWindow(GLint windowWidth, GLint windowHeight) {
    width = windowWidth;
    height = windowHeight;

    for (size_t i = 0; i < 1024; i++) {
        keys[i] = 0;
    }
}

int GLWindow::Initialize() {
    if (!glfwInit()) {
        printf("GLFW init failed...");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW window failed to create...");
        glfwTerminate();
        return 1;
    }

    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    glfwMakeContextCurrent(mainWindow);

    CreateCallbacks();
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("GLEW init failed...");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    // How much of the screen space is available to OpenGL
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    glfwSetWindowUserPointer(mainWindow, this);
}

void GLWindow::CreateCallbacks() {
    glfwSetKeyCallback(mainWindow, HandleKeys);
    glfwSetCursorPosCallback(mainWindow, HandleMouse);
}

GLfloat GLWindow::GetXChange() {
    GLfloat currChange;
    if (xChange >= 10000.0f || xChange <= -10000.0f) currChange = 0.0f;
    else currChange = xChange;

    xChange = 0.0f;
    return currChange;
}

GLfloat GLWindow::GetYChange() {
    GLfloat currChange;
    if (yChange >= 10000.0f || yChange <= -10000.0f) currChange = 0.0f;
    else currChange = yChange;

    yChange = 0.0f;
    return currChange;
}

void GLWindow::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
    GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            theWindow->keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            theWindow->keys[key] = false;
        }
    }
}

void GLWindow::HandleMouse(GLFWwindow* window, double xPos, double yPos) {
    GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

    if (theWindow->mouseFirstMoved) {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }

    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange = theWindow->lastY - yPos;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
}

GLWindow::~GLWindow() {
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}