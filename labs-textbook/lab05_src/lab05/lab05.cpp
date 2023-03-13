#include "lab05.h"
#include "scenerunner.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;
using glm::vec3;
using glm::vec4;

Lab05::Lab05() : teapot(14, glm::mat4(1.0f))
{}

void Lab05::initScene()
{
    compileAndLinkShader();

    glClearColor(0.5f,0.5f,0.5f,1.0f);

    glEnable(GL_DEPTH_TEST);

    // Set up the framebuffer object
    setupFBO();

    // Array for full-screen quad
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers

    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object

    glGenVertexArrays( 1, &fsQuad );
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer( (GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer( (GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray(2);  // Texture coordinates

    glBindVertexArray(0);

    GLuint programHandle = prog.getHandle();

    prog.setUniform("Light.Intensity", vec3(0.85f));
    prog.setUniform("RenderTex", 0);
    prog.setUniform("DepthTex", 1);

    pass1Index = glGetSubroutineIndex( programHandle, GL_FRAGMENT_SHADER, "FirstPass");
    pass2Index = glGetSubroutineIndex( programHandle, GL_FRAGMENT_SHADER, "SecondPass");

}

void Lab05::setupFBO()
{
    // Setting up the Frame Buffer Object will go into this function
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // Create the render texture object
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);
    #ifdef __APPLE__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    #else
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    // Create the depth texture object
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    #ifdef __APPLE__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    #else
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    // Bind the textures to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, depthTex, 0);
    // Set the targets for the fragment output variables
    GLenum drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(3, drawBuffers);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    } else {
        printf("Framebuffer is not complete.\n");
    }


    // The last call should be to revert to the default framebuffer
    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Lab05::update( float t )
{
}

void Lab05::render()
{
    prog.use();
    // Pass 1 (render with no blur)
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass1Index);
    float c = 2.0f;
    vec3 cameraPos(-3,3,7);
    view = glm::lookAt(cameraPos,vec3(0.0f),vec3(0.0f,1.0f,0.0f));
    prog.setUniform("Light.Position", view * vec4(20.0f, c * 5.25f, c * 7.5f, 1.0f));
    projection = glm::perspective(glm::radians(50.0f), (float)width/height, 0.1f, 100.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    glFinish();
    
    // Pass 2 (render with depth blur)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass2Index);
    view = mat4(1.0);
    model = mat4(1.0);
    projection = mat4(1.0);
    setMatrices();
    // Render the quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glFinish();

}

void Lab05::drawScene()
{
    vec3 color = vec3(0.7f,0.5f,0.3f);
    prog.setUniform("Material.Ka", color * 0.05f);
    prog.setUniform("Material.Kd", color);
    prog.setUniform("Material.Ks", vec3(0.9f,0.9f,0.9f));
    prog.setUniform("Material.Shininess", 150.0f);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f,0.0f,0.0f));
    model = glm::translate(model, vec3(-2.0f,0.0f,-2.0f) );
    setMatrices();
    teapot.render();
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f,0.0f,0.0f));
    model = glm::translate(model, vec3(4.0f,+5.0f,-2.0f) );
    setMatrices();
    teapot.render();
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f,0.0f,0.0f));
    model = glm::translate(model, vec3(10.0f,+10.0f,-2.0f) );
    setMatrices();
    teapot.render();
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f,0.0f,0.0f));
    model = glm::translate(model, vec3(16.0f,+15.0f,-2.0f) );
    setMatrices();
    teapot.render();

}

void Lab05::setMatrices()
{
    mat4 mv = view * model;
    mat4 bias;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    glm::mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void Lab05::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
}

void Lab05::compileAndLinkShader()
{
	try {
		prog.compileShader("shaders/lab05.vs");
		prog.compileShader("shaders/lab05.fs");
    	prog.link();
    	prog.use();

    } catch(GLSLProgramException &e ) {
    	cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}

int main(int argc, char *argv[])
{
	SceneRunner runner("Lab 05 - Depth Blur");
	std::unique_ptr<Scene> scene;
	scene = std::unique_ptr<Scene>( new Lab05() );

	return runner.run(std::move(scene));
}
