#include "scene.h"
#include "glslprogram.h"
#include "teapot.h"

#include "cookbookogl.h"

#include <glm/glm.hpp>

class Lab05 : public Scene
{
private:
    GLSLProgram prog;
    GLuint fboHandle, pass1Index, pass2Index, renderTex, depthTex;

    Teapot teapot;

    float gPivot;
    GLuint fsQuad;

    void setMatrices();
    void compileAndLinkShader();
    void setupFBO();
    void drawScene();

public:
    Lab05();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};