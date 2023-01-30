#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>

// the X and Z values are for drawing an icosahedron
#define IX 0.525731112119133606 
#define IZ 0.850650808352039932


GLuint program;

int lastTime;
int nFrames;

int colorScheme = 1;
int vertexScheme = 1;

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
	char *vsSource, *fsSource;
	GLuint vs,fs;

	vsSource = (char *)malloc(sizeof(char)*20000);
	fsSource = (char *)malloc(sizeof(char)*20000);

	vsSource[0]='\0';
	fsSource[0]='\0';

	program = glCreateProgram();

	readShader("vertex.vs",vsSource);
	readShader("fragment.fs",fsSource);

	vs = loadShader(vsSource,GL_VERTEX_SHADER);
	fs = loadShader(fsSource,GL_FRAGMENT_SHADER);

	glAttachShader(program,vs);
	glAttachShader(program,fs);

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
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		colorScheme = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		colorScheme = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		colorScheme = 3;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		vertexScheme = 1;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		vertexScheme = 2;
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		vertexScheme = 3;
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

    int i,j,c;

    GLint mvp_location;
	GLint time_location;

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
   	{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

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

    glBindBuffer(GL_ARRAY_BUFFER, vertpos_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    glBindBuffer(GL_ARRAY_BUFFER, vertcolor_buffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	glBindVertexArray(0);

    initShaders();

    glEnable(GL_DEPTH_TEST);

	float angle =0;
	
    vec3 cameraPos;
    mat4 mvp,view,projection;

    mvp_location = glGetUniformLocation(program,"MVP");
	time_location = glGetUniformLocation(program, "time");

	glClearColor(0.4,0.4,0.4,0);

	float tPrev, rotSpeed, t, deltaT;

	angle = glm::radians(140.0f);

    tPrev = 0;
    rotSpeed = glm::pi<float>()/8.0f;

	GLuint passThroughIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "passThrough");
	GLuint grayScaleIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "grayScale");
	GLuint timeFlowIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "timeFlow");

	GLuint passThroughVertexIndex = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "passThrough");
	GLuint shrinkObjIndex = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "shrinkObj");
	GLuint timeFlowVertexIndex = glGetSubroutineIndex(program, GL_VERTEX_SHADER, "timeFlow");

    while (!glfwWindowShouldClose(window))
    {
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

    	angle += rotSpeed * deltaT;
    	if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

	    cameraPos = vec3(2.0f * cos(angle), 1.5f, 2.0f * sin(angle));
    	view = glm::lookAt(cameraPos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));

    	projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.3f, 100.0f);

    	mvp = projection * view;

		if(colorScheme == 1) {
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &passThroughIndex);
		} else if(colorScheme == 2) {
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &grayScaleIndex);
		} else if(colorScheme == 3) {
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &timeFlowIndex);
		}

		if(vertexScheme == 1) {
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &passThroughVertexIndex);
		} else if(vertexScheme == 2) {
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &shrinkObjIndex);
		} else if(vertexScheme == 3) {
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &timeFlowVertexIndex);
		}

    	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &(mvp)[0][0]);

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
