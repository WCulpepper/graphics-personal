#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <math.h>

#include "teapotpatch.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

int tessLevel;

GLuint program;

int lastTime;
int nFrames;

void readShader(const char* fname, char *source)
{
	FILE *fp;
	fp = fopen(fname,"r");
	if (fp==NULL)
	{
		fprintf(stderr,"The shader file %s cannot be opened!\n",fname);
		glfwTerminate();
		exit(1);
	}
	char tmp[300];
	while (fgets(tmp,300,fp)!=NULL)
	{
		strcat(source,tmp);
		strcat(source,"\n");
	}
}

unsigned int loadShader(const char *source, unsigned int mode)
{
	GLuint id;
	id = glCreateShader(mode);

	glShaderSource(id,1,&source,NULL);

	glCompileShader(id);

	char error[1000];

	glGetShaderInfoLog(id,1000,NULL,error);
	printf("Compile status: \n %s\n",error);

	return id;
}

void initShaders()
{
	char *vsSource, *fsSource, *gsSource, *tcsSource, *tesSource;
	GLuint vs,fs,gs,tcs,tes;

	vsSource = (char *)malloc(sizeof(char)*20000);
	fsSource = (char *)malloc(sizeof(char)*20000);
	gsSource = (char *)malloc(sizeof(char)*20000);
	tcsSource = (char *)malloc(sizeof(char)*20000);
	tesSource = (char *)malloc(sizeof(char)*20000);

	vsSource[0]='\0';
	fsSource[0]='\0';
	gsSource[0]='\0';
	tcsSource[0]='\0';
	tesSource[0]='\0';

	program = glCreateProgram();

	readShader("shader/lab03.vs",vsSource);
	readShader("shader/lab03.fs",fsSource);
	readShader("shader/lab03.gs",gsSource);
	readShader("shader/lab03.tcs",tcsSource);
	readShader("shader/lab03.tes",tesSource);

	vs = loadShader(vsSource,GL_VERTEX_SHADER);
	fs = loadShader(fsSource,GL_FRAGMENT_SHADER);
	gs = loadShader(gsSource,GL_GEOMETRY_SHADER);
	tcs = loadShader(tcsSource,GL_TESS_CONTROL_SHADER);
	tes = loadShader(tesSource,GL_TESS_EVALUATION_SHADER);

	glAttachShader(program,vs);
	glAttachShader(program,fs);
	glAttachShader(program,gs);
	glAttachShader(program,tcs);
	glAttachShader(program,tes);

	glLinkProgram(program);

	glUseProgram(program);
	
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        tessLevel++;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        tessLevel--;
}

void showFPS(GLFWwindow* window) {
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
	    char ss[500] = {};
		std::string wTitle = "Lab03";
        nFrames++;
        if ( delta >= 1.0 ){ // If last update was more than 1 sec ago
            double fps = ((double)(nFrames)) / delta;
            sprintf(ss,"%s running at %lf FPS.",wTitle.c_str(),fps);
            glfwSetWindowTitle(window, ss);
            nFrames = 0;
            lastTime = currentTime;
        }
}

int main(void)
{
	
    GLFWwindow* window;

    int i,j,c;

    GLint mvp_location,time_location;

	lastTime = 0;
	nFrames = 0;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


    window = glfwCreateWindow(800, 600, "Lab03", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

	gladLoadGL();

	const GLubyte *renderer = glGetString( GL_RENDERER );
	const GLubyte *vendor = glGetString( GL_VENDOR );
	const GLubyte *version = glGetString( GL_VERSION );
	const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor            : %s\n", vendor);
	printf("GL Renderer          : %s\n", renderer);
	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version         : %s\n", glslVersion);
	
    initShaders();

    TeapotPatch teapot;

    glEnable(GL_DEPTH_TEST);

	float angle = 0;
    tessLevel = 8;
	
    vec3 cameraPos;
    mat4 mvp,view,projection,model;

    mvp_location = glGetUniformLocation(program,"MVP");

    glUniform1i(glGetUniformLocation(program,"TessLevel"), tessLevel);
    glUniform1f(glGetUniformLocation(program,"LineWidth"), 0.8f);
    glUniform4f(glGetUniformLocation(program,"LineColor"), 0.05f,0.0f,0.05f,1.0f);
    glUniform4f(glGetUniformLocation(program,"LightPosition"), 0.0f,0.0f,0.0f,1.0f);
    glUniform3f(glGetUniformLocation(program,"LightIntensity"), 1.0f,1.0f,1.0f);
    glUniform3f(glGetUniformLocation(program,"Kd"), 0.9f,0.9f,1.0f);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

	glClearColor(0.5,0.5,0.5,1.0);

	float tPrev, rotSpeed, t, deltaT;

    angle = glm::pi<float>() / 3.0f;

    tPrev = 0;
    rotSpeed = glm::pi<float>()/8.0f;
	
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);


        float w2 = width / 2.0f;
        float h2 = height / 2.0f;
        mat4 viewport = mat4( vec4(w2,0.0f,0.0f,0.0f),
                     vec4(0.0f,h2,0.0f,0.0f),
                     vec4(0.0f,0.0f,1.0f,0.0f),
                     vec4(w2+0, h2+0, 0.0f, 1.0f));


        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		t = float(glfwGetTime());
		deltaT = t - tPrev;
    	if(tPrev == 0.0f) deltaT = 0.0f;
    	tPrev = t;

    	angle += rotSpeed * deltaT;
    	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

        cameraPos = vec3(4.25f * cos(angle),3.0f,4.25f * sin(angle));
        view = glm::lookAt(cameraPos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));

        model = mat4(1.0f);
        model = glm::translate(model, vec3(0.0f,-1.5f,0.0f));
        model = glm::rotate(model,glm::radians(-90.0f), vec3(1.0f,0.0f,0.0f));

    	projection = glm::perspective(glm::radians(60.0f), (float)width/height, 0.3f, 100.0f);

    	mvp = projection * view * model;

        mat4 mv = view * model;
        mat3 nm = mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) );

    	glUniformMatrix4fv(glGetUniformLocation(program,"ModelViewMatrix"), 1, GL_FALSE, &(mv)[0][0]);
    	glUniformMatrix3fv(glGetUniformLocation(program,"NormalMatrix"), 1, GL_FALSE, &(nm)[0][0]);
    	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &(mvp)[0][0]);
    	glUniformMatrix4fv(glGetUniformLocation(program,"ViewportMatrix"), 1, GL_FALSE, &(viewport)[0][0]);

        glUniform1i(glGetUniformLocation(program,"TessLevel"), tessLevel);

        teapot.render();

		showFPS(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
