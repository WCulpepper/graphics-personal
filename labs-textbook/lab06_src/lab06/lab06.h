#include "scene.h"
#include "glslprogram.h"
#include "sphere.h"
#include "skybox.h"

#include <glm/glm.hpp>

class Lab06 : public Scene
{
private:
	GLSLProgram prog;
    GLSLProgram skyProg;

    Sphere sphere;
    SkyBox sky;

    float angle, tPrev, rotSpeed;

    void setMatrices(GLSLProgram &);
    void compileAndLinkShader();

public:
    Lab06();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};