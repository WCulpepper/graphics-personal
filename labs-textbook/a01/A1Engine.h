#ifndef A1Engine_H
#define A1Engine_H

#include "scene.h"

#include "cookbookogl.h"
#include <string>

class A1Engine : public Scene
{
private:
    GLuint vaoHandle;
    GLuint programHandle;

    float angle, tPrev, rotSpeed;

    void linkMe(GLint vertShader, GLint fragShader);
	void compileShaderProgram();
    void writeShaderBinary();
	void loadShaderBinary(GLint);
	void loadSpirvShader();

    std::string getShaderInfoLog(GLuint shader);
    std::string getProgramInfoLog(GLuint program);

public:
    A1Engine();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);

};

#endif // A1Engine_H
