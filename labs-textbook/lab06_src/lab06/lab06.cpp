#include "lab06.h"
#include "scenerunner.h"
#include "texture.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::mat4;

Lab06::Lab06() : angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f),
                                       sphere(2, 150, 150), sky(100.0f)
{ }

void Lab06::initScene()
{
    compileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    projection = mat4(1.0f);

    angle = glm::radians(90.0f);

    GLuint cubeTex = Texture::loadCubeMap("images/cube");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    skyProg.use();
    skyProg.setUniform("SkyBoxTex", 0);

    prog.use();

    prog.setUniform("CubeMapTex", 0);
}

void Lab06::update( float t )
{
	float deltaT = t - tPrev;
	if(tPrev == 0.0f) deltaT = 0.0f;
	tPrev = t;

    angle += rotSpeed * deltaT;
	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}

void Lab06::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 cameraPos = vec3( 7.0f * cos(angle), 1.0f, 7.0f * sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));

    skyProg.use();
    model = mat4(1.0f);
    setMatrices(skyProg);
    sky.render();
	// Draw scene
	prog.use();

    glm::vec4 worldLight = glm::vec4(-15.0f,5.0f,-2.0f,1.0f);

    prog.setUniform("Material.Kd", 0.3f, 0.6f, 1.0f);
    prog.setUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Light.Position", view * worldLight );
    prog.setUniform("Material.Ka", 0.3f, 0.6f, 1.0f);
    prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    prog.setUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Shininess", 50.0f);
    prog.setUniform("WorldCameraPosition", cameraPos);
    prog.setUniform("ReflectFactor", 0.5f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f,0.5f,0.0f));
    setMatrices(prog);
    sphere.render();
}

void Lab06::setMatrices(GLSLProgram &p)
{
    mat4 mv = view * model;
    p.setUniform("ModelViewMatrix", mv);
    p.setUniform("NormalMatrix",
                    glm::mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    p.setUniform("MVP", projection * mv);
    p.setUniform("ModelMatrix", model);
}

void Lab06::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(50.0f), (float)w/h, 0.3f, 100.0f);
}

void Lab06::compileAndLinkShader()
{
	try {
        prog.compileShader("shaders/lab06.vs");
		prog.compileShader("shaders/lab06.fs");
    	prog.link();

        skyProg.compileShader("shaders/skybox.vs");
        skyProg.compileShader("shaders/skybox.fs");
        skyProg.link();

    	prog.use();
    } catch(GLSLProgramException & e) {
 		cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}

int main(int argc, char *argv[])
{
	SceneRunner runner("Lab 06 - Environment Mapping");
	std::unique_ptr<Scene> scene;
	scene = std::unique_ptr<Scene>( new Lab06() );

	return runner.run(std::move(scene));
}
