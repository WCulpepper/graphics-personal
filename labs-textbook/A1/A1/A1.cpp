#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>

// the X and Z values are for drawing an icosahedron
#define IX 0.525731112119133606 
#define IZ 0.850650808352039932


GLuint gouraudProgram;
GLuint phongProgram;

bool useGouraud = true;

int lastTime;
int nFrames;

int shaderSubroutine = 1;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;

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
	char *vsSource, *fsSource, *vsPhongSource, *fsPhongSource;
	GLuint vs,fs, vsPhong, fsPhong;

	vsSource = (char *)malloc(sizeof(char)*20000);
	fsSource = (char *)malloc(sizeof(char)*20000);

	vsPhongSource = (char *)malloc(sizeof(char)*20000);
	fsPhongSource = (char *)malloc(sizeof(char)*20000);

	vsSource[0]='\0';
	fsSource[0]='\0';

	vsPhongSource[0]='\0';
	fsPhongSource[0]='\0';

	gouraudProgram = glCreateProgram();

	readShader("vertex_gouraud.vs",vsSource);
	readShader("fragment_gouraud.fs",fsSource);

	vs = loadShader(vsSource,GL_VERTEX_SHADER);
	fs = loadShader(fsSource,GL_FRAGMENT_SHADER);

	glAttachShader(gouraudProgram,vs);
	glAttachShader(gouraudProgram,fs);

	glLinkProgram(gouraudProgram);

	phongProgram = glCreateProgram();

	readShader("vertex_phong.vs",vsPhongSource);
	readShader("fragment_phong.fs",fsPhongSource);

	vsPhong = loadShader(vsPhongSource,GL_VERTEX_SHADER);
	fsPhong = loadShader(fsPhongSource,GL_FRAGMENT_SHADER);

	glAttachShader(phongProgram,vsPhong);
	glAttachShader(phongProgram,fsPhong);

	glLinkProgram(phongProgram);

	glUseProgram(phongProgram);
	
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		useGouraud = true;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		useGouraud = false;
}

void showFPS(GLFWwindow* window) {
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
	    char ss[500] = {};
		std::string wTitle = "Icosahedron";
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

    GLuint vertpos_buffer;
    GLuint vertcolor_buffer;
    GLuint index_buffer;
    GLuint VAO;

	GLuint vertpos_base_buffer;
	GLuint vertcolor_base_buffer;
	GLuint normal_base_buffer;
	GLuint groundVAO;

    int i,j,c;

    GLint mvp_location;
	GLint time_location;
	GLint cameraPos_location;
	GLint lightPos_location;

	lastTime = 0;
	nFrames = 0;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(1200, 600, "Icosahedron", NULL, NULL);
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
	

	// These are the 12 vertices for the icosahedron
	static GLfloat vdata[12][3] = {    
		{-IX, 0.0, IZ}, {IX, 0.0, IZ}, {-IX, 0.0, -IZ}, {IX, 0.0, -IZ},    
		{0.0, IZ, IX}, {0.0, IZ, -IX}, {0.0, -IZ, IX}, {0.0, -IZ, -IX},    
		{IZ, IX, 0.0}, {-IZ, IX, 0.0}, {IZ, -IX, 0.0}, {-IZ, -IX, 0.0} 
	};

	// vertex colors
	static GLfloat cdata[12][3] = {    
		{0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0},    
		{0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0},
		{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0} 
	};

	// These are the 20 faces.  Each of the three entries for each 
	// vertex gives the 3 vertices that make the face.
	static GLint tindices[20][3] = { 
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
		{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
		{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} 
	};

	static GLfloat vdata_base[4][3] = {
		{-2.0,-1.0,-2.0}, {2.0,-1.0,-2.0}, {2.0,-1.0,2.0}, {-2.0,-1.0,2.0}
	};

	static GLfloat cdata_base[4][3] = {    
		{0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}
	};

	static GLfloat normaldata_base[4][3] {
		{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}
	};

    glGenBuffers(1, &vertpos_buffer);
    glGenBuffers(1, &vertcolor_buffer);
    glGenBuffers(1, &index_buffer);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
    glBindBuffer(GL_ARRAY_BUFFER, vertpos_buffer);
    glBufferData(GL_ARRAY_BUFFER, 36*sizeof(float), (void *)&(vdata[0][0]), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertcolor_buffer);
    glBufferData(GL_ARRAY_BUFFER, 36*sizeof(float), (void *)&(cdata[0][0]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 60*sizeof(int), (void *)&(tindices[0][0]), GL_STATIC_DRAW);

	// position attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertpos_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	// color attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertcolor_buffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );


	glGenBuffers(1, &vertpos_base_buffer);
    glGenBuffers(1, &vertcolor_base_buffer);
	// ground VAO/VBO
	glGenVertexArrays(1, &groundVAO);
    glBindVertexArray(groundVAO);

	
    // glGenBuffers(1, &normal_base_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertpos_base_buffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), (void* )&(vdata_base[0][0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertcolor_base_buffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), (void* )&(cdata_base[0][0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertpos_base_buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, vertcolor_base_buffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	glBindBuffer(GL_ARRAY_BUFFER, normal_base_buffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), (void* )&(normaldata_base[0][0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	glBindVertexArray(0);
	

    initShaders();

    glEnable(GL_DEPTH_TEST);

	float angle =0;
	
    vec3 cameraPos, lightPos;
    mat4 mvp,view,projection, model;

	

    mvp_location = glGetUniformLocation(gouraudProgram,"MVP");
	time_location = glGetUniformLocation(gouraudProgram, "time");
	cameraPos_location = glGetUniformLocation(gouraudProgram, "cameraPos");
	lightPos_location = glGetUniformLocation(gouraudProgram, "lightPos");
	
	lightPos = vec3(3.0f, 3.0f, 3.0f);
	glUniform3fv(lightPos_location, 1, &(lightPos)[0]);

	glClearColor(0.2,0.2,0.2,0);

	float tPrev, rotSpeed, t, deltaT;

	angle = glm::radians(140.0f);

    tPrev = 0;
    rotSpeed = glm::pi<float>()/800.0f;

	// GLuint passThroughIndexGouraud = glGetSubroutineIndex(gouraudProgram, GL_VERTEX_SHADER, "passThrough");
	// GLuint passThroughIndexPhong = glGetSubroutineIndex(phongProgram, GL_FRAGMENT_SHADER, "passThrough");

	cameraPos = vec3(2.0f, 1.5f, 2.0f);
	glUniform3fv(cameraPos_location, 1, &(cameraPos)[0]);
	view = glm::lookAt(cameraPos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
	
    while (!glfwWindowShouldClose(window))
    {
		
		if(useGouraud) {
			glUseProgram(gouraudProgram);
			std::cout << "Using the Gouraud Shader\n";
		} else {
			glUseProgram(phongProgram);
			std::cout << "Using the Phong Shader\n";
		}

		model = mat4(1.0f);
		glUniform1f(time_location, glfwGetTime());
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		t = float(glfwGetTime());
		deltaT = t - tPrev;
    	if(tPrev == 0.0f) deltaT = 0.0f;
    	tPrev = t;
    	
    	angle += 0.005f;
		if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

	    // cameraPos = vec3(2.0f * cos(angle), 1.5f, 2.0f * sin(angle));
		

    	projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.3f, 100.0f);
		model = glm::rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));
    	mvp = projection * view * model;

    	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &(mvp)[0][0]);

		// glBindVertexArray(groundVAO);
		// glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);

	    glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT,0);
    	glBindVertexArray(0);

		showFPS(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
