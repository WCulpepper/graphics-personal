#include "scenesilhouette.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;

SceneSilhouette::SceneSilhouette() : angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f) {
    ogre = ObjMesh::loadWithAdjacency("../media/bs_ears.obj");
}

void SceneSilhouette::initScene()
{
    compileAndLinkShader();

    glClearColor(0.5f,0.5f,0.5f,1.0f);

    glEnable(GL_DEPTH_TEST);

	angle = glm::half_pi<float>();

    ///////////// Uniforms ////////////////////
    prog.setUniform("EdgeWidth", 0.015f);
    prog.setUniform("PctExtend", 0.25f);
    prog.setUniform("LineColor", vec4(0.05f,0.0f,0.05f,1.0f));
    prog.setUniform("Material.Kd", 0.7f, 0.5f, 0.2f);
    prog.setUniform("Light.Position", vec4(0.0f,0.0f,0.0f,1.0f));
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);
    /////////////////////////////////////////////
}


void SceneSilhouette::update( float t )
{
	float deltaT = t - tPrev;
	if(tPrev == 0.0f) deltaT = 0.0f;
	tPrev = t;

    angle += rotSpeed * deltaT;
	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void SceneSilhouette::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos(1.5f * cos(angle),0.0f,1.5f * sin(angle));
    view = glm::lookAt(cameraPos,
                       vec3(0.0f,-0.2f,0.0f),
                       vec3(0.0f,1.0f,0.0f));

    model = mat4(1.0f);
    setMatrices();
    ogre->render();

    glFinish();
}

void SceneSilhouette::setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void SceneSilhouette::resize(int w, int h)
{
    glViewport(0,0,w,h);
    float c = 1.5f;
    projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f *c, 0.3f*c, 0.1f, 100.0f);
}

void SceneSilhouette::compileAndLinkShader()
{
	try {
		prog.compileShader("shader/silhouette.vs",GLSLShader::VERTEX);
		prog.compileShader("shader/silhouette.fs",GLSLShader::FRAGMENT);
		prog.compileShader("shader/silhouette.gs",GLSLShader::GEOMETRY);
    	prog.link();
    	prog.use();
    } catch(GLSLProgramException &e ) {
    	cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}
