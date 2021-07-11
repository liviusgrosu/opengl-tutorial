#include "GLWindow.h"
GLWindow::GLWindow() {
    width = 800;
    height = 600;

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

    mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW window failed to create...");
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Handle Key + Mouse Input
    CreateCallbacks();
    // Hide the cursor and keep in the middle
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extensions features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("GLEW init failed...");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Check the depth information so that fragments are not overlapping incorrectly
    glEnable(GL_DEPTH_TEST);

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    glfwSetWindowUserPointer(mainWindow, this);
}

void GLWindow::CreateCallbacks() {
    // Create the input callbacks for the user pointer object
    glfwSetKeyCallback(mainWindow, HandleKeys);
    glfwSetCursorPosCallback(mainWindow, HandleMouse);
}

GLfloat GLWindow::getXChange() {
    GLfloat change = xChange;
    xChange = .0f;
    return change;
}

GLfloat GLWindow::getYChange() {
    GLfloat change = yChange;
    yChange = .0f;
    return change;
}

void GLWindow::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
    GLWindow* theWindow = static_cast<GLWindow*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // Exit the application
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
    // We do this to stop inverted y movement
    theWindow->yChange = theWindow->lastY - yPos;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
}

GLWindow::~GLWindow() {
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}