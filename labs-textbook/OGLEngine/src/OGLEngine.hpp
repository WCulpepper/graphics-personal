#ifndef OGL_ENGINE_HPP
#define OGL_ENGINE_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/noise.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <math.h>
#include <iostream>


#include "ArcballCam.hpp"
#include "FreeCam.hpp"
#include "teapotpatch.h"
#include "cube.h"
#include "icosahedron.h"
#include "sphere.h"
#include "Materials.hpp"

class OGLEngine {
public: 
    // Constructor/Destructor
    OGLEngine(int majorVersion, int minorVersion, int windowWidth, int windowHeight, const char* windowName);
    OGLEngine();
    ~OGLEngine();

    void initialize();
    void run();
    void shutdown();

    // Event handlers
    void keyEventHandler(GLint key, GLint action);
    void mbEventHandler(GLint button, GLint action);
    void cursorPosEventHandler(glm::vec2 mousePosition);
    void scrollWheelEventHandler(double offset);
    void windowResizeHandler(int width, int height);

    static constexpr GLfloat MOUSE_UNINIT = -9999.0f;

    void setCurrentWindowSize(const int windowWidth, const int windowHeight) { _windowWidth = windowWidth; _windowHeight = windowHeight; }
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

    bool _isInitialized;
    bool _isCleanedUp;
    
    void _setupGLFW();
    void _setupOGL();
    void _setupShaders();
    void _setupBuffers();
    void _setupTextures();
    void _setupScene();

    void _showFPS(GLFWwindow* window);

    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx, glm::mat4 viewportMtx);
    void _updateScene();
    void _setMaterial(Materials::Material m);

    void _cleanupBuffers();
    void _cleanupShaders();
    void _cleanupTextures();
    void _cleanupScene();
    void _cleanupOGL();
    void _cleanupGLFW();

    unsigned int _errorCode;
    int _majorVersion;
    int _minorVersion;
    int _windowWidth;
    int _windowHeight;
    int _windowResizable;
    char* _windowTitle;
    GLFWwindow* _window;

    bool DEBUG = false;

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
    GLfloat _cameraAngle;

    int _selectedCam;
    GLfloat _rotSpeed;

    GLfloat _deltaT;
    GLfloat _tPrev;

    GLuint _wireframeProgram;
    GLuint _teapotProgram;
    GLuint _rtProgram;

    glm::vec3 _lightColor = glm::vec3(1.0,1.0,1.0);

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

    struct WFUniformLocations {
        GLint mvpMtx;
        GLint mvMtx;
        GLint modelMtx;
        GLint normalMtx;
        GLint viewportMtx;
        GLint cameraPos;
        GLint lightColor;
        GLint lineWidth;
        GLint lineColor;
        GLint materialIndex;
        GLint wfBoolLocation;
    } _wfUniformLocations;

    struct WFSubroutineLocations {
        GLuint phongSpec;
        GLuint blinnPhongSpec;
    } _wfSubroutineLocations;

    struct TessUniformLocations {
        GLint mvpMtx;
        GLint mvMtx;
        GLint modelMtx;
        GLint normalMtx;
        GLint viewportMtx;
        GLint cameraPos;
        GLint lineWidth;
        GLint lineColor;
        GLint tessLevel;
        GLint lightIntensity;
        GLint lightPos;
        GLint kd;
    } _tessUniformLocations;

    struct RTUniformLocations {
        GLint projectionMtx;
        GLint viewMtx;
        GLint mvpMtx;
        GLint normalMtx;
        GLint cameraPos;
        GLint cameraUp;
        GLint cameraGaze;
    } _rtUniformLocations;

    struct SceneObjects {
        TeapotPatch* teapot;
        Cube* cube;
        Icosahedron* ico;
        Sphere* sphere;
    } objects;

    struct Vertex {
        GLfloat x, y, z, normX, normY, normZ, texU, texV;
    };

    struct Texture {
        GLuint id;
        const char* type;
    };

};

void readShader(const char* fname, char *source);
unsigned int loadShader(const char *source, unsigned int mode);
void showFPS(GLFWwindow* window);

int storeTex( GLubyte * data, int w, int h );
int generate2DTex(float baseFreq = 4.0f, float persistence = 0.5f, int w = 128, int h = 128, bool periodic = false);
int generatePeriodic2DTex(float baseFreq = 4.0f, float persist = 0.5f, int w = 128, int h = 128);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void error_callback(int error, const char* description);
void window_resize_callback(GLFWwindow* window, int width, int height);

inline void printOGLInfo() {
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

	fprintf( stdout, "\n[INFO]: /--------------------------------------------------------\\\n" );
	fprintf( stdout, "[INFO]: | OpenGL Information                                     |\n" );
	fprintf( stdout, "[INFO]: |--------------------------------------------------------|\n" );
	fprintf( stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION) );
	fprintf( stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER) );
	fprintf( stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR) );
	fprintf( stdout, "[INFO]: |   Shading Version: %35s |\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );
    fprintf( stdout, "\n[INFO]: /--------------------------------------------------------\\\n" );
}

#endif // OGL_ENGINE_HPP