#ifndef OGL_ENGINE_HPP
#define OGL_ENGINE_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ArcballCam.hpp>
#include <FreeCam.hpp>

class OGLEngine {
public: 
    // Constructor/Destructor
    OGLEngine(int OPENGL_MAJOR, int OPENGL_MINOR, int WINDOW_WIDTH, int WINDOW_HEIGHT, const char* WINDOW_NAME);
    ~OGLEngine();

    void initialize();
    void run();

    // Event handlers
    void keyEventHandler(GLint key, GLint action);
    void mbEventHandler(GLint button, GLint action);
    void cursorPosEventHandler(glm::vec2 mousePosition);
    void scrollWheelEventHandler(double offset);

    static constexpr GLfloat MOUSE_UNINIT = -9999.0f;

    void setCurrentWindowSize(const int WINDOW_WIDTH, const int WINDOW_HEIGHT) { _windowWidth = WINDOW_WIDTH; _windowHeight = WINDOW_HEIGHT; }
    /// \desc Return the height of the window
    int getWindowHeight() const noexcept { return _windowHeight; }
    /// \desc Return the width of the window
    int getWindowWidth() const noexcept { return _windowWidth; }
    /// \desc Return the window object
    GLFWwindow* getWindow() const noexcept { return _window; }

    /// \desc Tell our engine's window to close
    void setWindowShouldClose() { glfwSetWindowShouldClose(_window, GLFW_TRUE); }

    unsigned int getError() noexcept {
        unsigned int storedErrorCode = _errorCode;  // store current error code
        _errorCode = OPENGL_ENGINE_ERROR_NO_ERROR;  // reset error code
        return storedErrorCode;                     // return previously stored error code
    }

    static const unsigned int OPENGL_ENGINE_ERROR_NO_ERROR      = 0;
    static const unsigned int OPENGL_ENGINE_ERROR_GLFW_INIT     = 1;
    static const unsigned int OPENGL_ENGINE_ERROR_GLFW_WINDOW   = 2;
    static const unsigned int OPENGL_ENGINE_ERROR_GLEW_INIT     = 3;

private:
    
    void _setupGLFW();
    void _setupOGL();
    void _setupShaders();
    void _setupBuffers();
    void _setupTextures();
    void _setupScene();

    void _cleanupBuffers();
    void _cleanupShaders();
    void _cleanupScene();

    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx);
    void _updateScene();

    // keyboard input tracking
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    GLboolean _keys[NUM_KEYS];

    glm::vec2 _mousePos;
    GLint _leftMouseButtonState;

    // Cameras and their attributes
    ArcballCam* _arcballCam;
    FreeCam* _freeCam;
    glm::vec2 _freeCamSpeed;
    glm::vec3 _cameraPos;
    int _selectedCam;

    static constexpr int NUM_VAOS = 4;
    enum VAO_ID {
        GROUND_PLANE = 0,
        ICOSAHEDRON = 1,
        CUBE = 2,
        TEAPOT = 3
    };
    // Vertex Array Objects
    GLuint _vaos[NUM_VAOS];
    // Vertex Buffer Objects
    GLuint _vbos[NUM_VAOS];
    // Index Buffer Objects
    GLuint _ibos[NUM_VAOS];
    // number of points that make up our VAO
    GLsizei _numVAOPoints[NUM_VAOS];
protected:
    unsigned int _errorCode;
    int _majorVersion;
    int _minorVersion;
    int _windowWidth;
    int _windowHeight;
    int _windowResizable;
    char* _windowTitle;
    GLFWwindow* _window;

    static void _windowResizeCallback(GLFWwindow* window, int width, int height);
};

#endif // OGL_ENGINE_HPP