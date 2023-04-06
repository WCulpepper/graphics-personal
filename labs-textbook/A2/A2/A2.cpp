#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>

#include "teapotdata.h"
#include "teapotpatch.h"

// the X and Z values are for drawing an icosahedron
#define IX 0.525731112119133606 
#define IZ 0.850650808352039932


GLuint phongProgram;
GLuint teapotProgram;

bool usePhongSpec = true;

int lastTime;
int nFrames;
int tessLevel = 8;
int modelToRender = 1;

int shaderSubroutine = 1;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::vec2;
using namespace glm;

GLint leftMouseButtonState;
glm::vec2 mousePosition = vec2(-1.0,-1.0);

// float perlin(float x, float y, float z) {
// 	float x_cube = x % 1.0f;
// 	float y_cube = y % 1.0f;
// 	float z_cube = z % 1.0f;

// 	return 0.0f;
// }

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
	char *vsSource_wireframe, *fsSource_wireframe, *gsSource_wireframe, 
	*vsSource_teapot, *fsSource_teapot, *gsSource_teapot, *tcsSource_teapot, *tesSource_teapot;

	GLuint vsWireframe, gsWireframe, fsWireframe, 
	vsTeapot, gsTeapot, fsTeapot, tcsTeapot, tesTeapot;

	vsSource_wireframe = (char *)malloc(sizeof(char)*20000);
	gsSource_wireframe = (char *)malloc(sizeof(char)*20000);
	fsSource_wireframe = (char *)malloc(sizeof(char)*20000);

	vsSource_teapot = (char *)malloc(sizeof(char)*20000);
	gsSource_teapot = (char *)malloc(sizeof(char)*20000);
	fsSource_teapot = (char *)malloc(sizeof(char)*20000);
	tcsSource_teapot = (char *)malloc(sizeof(char)*20000);
	tesSource_teapot = (char *)malloc(sizeof(char)*20000);


	vsSource_wireframe[0]='\0';
	gsSource_wireframe[0]='\0';
	fsSource_wireframe[0]='\0';

	vsSource_teapot[0]='\0';
	gsSource_teapot[0]='\0';
	fsSource_teapot[0]='\0';
	tcsSource_teapot[0]='\0';
	tesSource_teapot[0]='\0';

	phongProgram = glCreateProgram();

	readShader("shader/wireframe_vertex.vs",vsSource_wireframe);
	readShader("shader/wireframe_fragment.fs",fsSource_wireframe);
	readShader("shader/wireframe_geometry.gs",gsSource_wireframe);

	readShader("shader/teapot_vertex.vs",vsSource_teapot);
	readShader("shader/teapot_fragment.fs",fsSource_teapot);
	readShader("shader/teapot_geometry.gs",gsSource_teapot);
	readShader("shader/teapot.tcs",tcsSource_teapot);
	readShader("shader/teapot.tes",tesSource_teapot);

	vsWireframe = loadShader(vsSource_wireframe,GL_VERTEX_SHADER);
	fsWireframe = loadShader(fsSource_wireframe,GL_FRAGMENT_SHADER);
	gsWireframe = loadShader(gsSource_wireframe,GL_GEOMETRY_SHADER);

	vsTeapot = loadShader(vsSource_teapot,GL_VERTEX_SHADER);
	fsTeapot = loadShader(fsSource_teapot,GL_FRAGMENT_SHADER);
	gsTeapot = loadShader(gsSource_teapot,GL_GEOMETRY_SHADER);
	tcsTeapot = loadShader(tcsSource_teapot, GL_TESS_CONTROL_SHADER);
	tesTeapot = loadShader(tesSource_teapot, GL_TESS_EVALUATION_SHADER);

	glAttachShader(phongProgram,vsWireframe);
	glAttachShader(phongProgram,fsWireframe);
	glAttachShader(phongProgram,gsWireframe);

	glAttachShader(teapotProgram,vsTeapot);
	glAttachShader(teapotProgram,fsTeapot);
	glAttachShader(teapotProgram,gsTeapot);
	glAttachShader(teapotProgram,tcsTeapot);
	glAttachShader(teapotProgram,tesTeapot);

	glLinkProgram(phongProgram);
	// glLinkProgram(teapotProgram); 

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
		modelToRender = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		modelToRender = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		modelToRender = 3;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		usePhongSpec = true;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		usePhongSpec = false;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if(button == GLFW_MOUSE_BUTTON_LEFT) {
		leftMouseButtonState = action;
	} 
}
 

static void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	
	if(mousePosition.x < 0 && mousePosition.y < 0) {
		mousePosition = vec2(x, y);
	}

	if(leftMouseButtonState == GLFW_PRESS) {

	}
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

    GLuint ico_vertpos_buffer;
	GLuint ico_normal_buffer;
    GLuint ico_index_buffer;
    GLuint icoVAO;

	GLuint cube_vertpos_buffer;
	GLuint cube_normal_buffer;
	GLuint cube_index_buffer;
	GLuint cubeVAO;

	GLuint teapot_patches_buffer;
	GLuint teapot_controlPoints_buffer;
	GLuint teapotVAO;

	GLuint vertpos_base_buffer;
	GLuint normal_base_buffer;
	GLuint groundVAO;

    int i,j,c;

	GLint mvp_location_phong, model_location_phong, mv_location_phong;
	GLint normal_location_phong;
	GLint viewport_location;
	GLint cameraPos_location_phong;
	GLint lineWidth_location, lineColor_location;
	GLint tesslevel_location;

	lastTime = 0;
	nFrames = 0;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    window = glfwCreateWindow(1200, 600, "Icosahedron", NULL, NULL);
    if (!window)
    {
		std::cout << "Error setting up winodow\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwMakeContextCurrent(window);

	std::cout << "Setup callbacks\n";
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
	
	std::cout << "Setting up buffers...\n";
	// These are the 12 vertices for the icosahedron
	static GLfloat vdata_ico[12][3] = {    
		{-IX, 0.0, IZ}, {IX, 0.0, IZ}, {-IX, 0.0, -IZ}, {IX, 0.0, -IZ},    
		{0.0, IZ, IX}, {0.0, IZ, -IX}, {0.0, -IZ, IX}, {0.0, -IZ, -IX},    
		{IZ, IX, 0.0}, {-IZ, IX, 0.0}, {IZ, -IX, 0.0}, {-IZ, -IX, 0.0} 
	};

	// These are the 20 faces.  Each of the three entries for each 
	// vertex gives the 3 vertices that make the face.
	static GLint tindices[20][3] = { 
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
		{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
		{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} 
	};

	static GLfloat vdata_cube[] = {
            -0.5, -0.5, -0.5,
            0.5, -0.5, -0.5,
            0.5,  0.5, -0.5,
            -0.5, 0.5, -0.5,
            -0.5, -0.5, 0.5, 
            0.5, -0.5, 0.5,
            0.5, 0.5, 0.5,
            -0.5, 0.5, 0.5 };
	
	int cube_indices[] = {
            0, 2, 1, 0, 3, 2,
            1, 2, 5, 5, 2, 6, 
            2, 7, 6, 3, 7, 2, 
            0, 1, 4, 1, 5, 4, 
            4, 5, 6, 4, 6, 7, 
            0, 4, 3, 4, 7, 3 };

	int teapot_patches[][16] =
	{
		/* rim */
		{102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
		/* body */
		{12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
		{24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
		/* lid */
		{96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,},
		{0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117},
		/* bottom */
		{118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37},
		/* handle */
		{41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56},
		{53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67},
		/* spout */
		{68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
		{80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95}
	};


	float teapot_controlPoints[][3] =
	{
		{0.2f, 0.f, 2.7f},
		{0.2f, -0.112f, 2.7f},
		{0.112f, -0.2f, 2.7f},
		{0.f, -0.2f, 2.7f},
		{1.3375f, 0.f, 2.53125f},
		{1.3375f, -0.749f, 2.53125f},
		{0.749f, -1.3375f, 2.53125f},
		{0.f, -1.3375f, 2.53125f},
		{1.4375f, 0.f, 2.53125f},
		{1.4375f, -0.805f, 2.53125f},
		{0.805f, -1.4375f, 2.53125f},
		{0.f, -1.4375f, 2.53125f},
		{1.5f, 0.f, 2.4f},
		{1.5f, -0.84f, 2.4f},
		{0.84f, -1.5f, 2.4f},
		{0.f, -1.5f, 2.4f},
		{1.75f, 0.f, 1.875f},
		{1.75f, -0.98f, 1.875f},
		{0.98f, -1.75f, 1.875f},
		{0.f, -1.75f, 1.875f},
		{2.f, 0.f, 1.35f},
		{2.f, -1.12f, 1.35f},
		{1.12f, -2.f, 1.35f},
		{0.f, -2.f, 1.35f},
		{2.f, 0.f, 0.9f},
		{2.f, -1.12f, 0.9f},
		{1.12f, -2.f, 0.9f},
		{0.f, -2.f, 0.9f},
		{-2.f, 0.f, 0.9f},
		{2.f, 0.f, 0.45f},
		{2.f, -1.12f, 0.45f},
		{1.12f, -2.f, 0.45f},
		{0.f, -2.f, 0.45f},
		{1.5f, 0.f, 0.225f},
		{1.5f, -0.84f, 0.225f},
		{0.84f, -1.5f, 0.225f},
		{0.f, -1.5f, 0.225f},
		{1.5f, 0.f, 0.15f},
		{1.5f, -0.84f, 0.15f},
		{0.84f, -1.5f, 0.15f},
		{0.f, -1.5f, 0.15f},
		{-1.6f, 0.f, 2.025f},
		{-1.6f, -0.3f, 2.025f},
		{-1.5f, -0.3f, 2.25f},
		{-1.5f, 0.f, 2.25f},
		{-2.3f, 0.f, 2.025f},
		{-2.3f, -0.3f, 2.025f},
		{-2.5f, -0.3f, 2.25f},
		{-2.5f, 0.f, 2.25f},
		{-2.7f, 0.f, 2.025f},
		{-2.7f, -0.3f, 2.025f},
		{-3.f, -0.3f, 2.25f},
		{-3.f, 0.f, 2.25f},
		{-2.7f, 0.f, 1.8f},
		{-2.7f, -0.3f, 1.8f},
		{-3.f, -0.3f, 1.8f},
		{-3.f, 0.f, 1.8f},
		{-2.7f, 0.f, 1.575f},
		{-2.7f, -0.3f, 1.575f},
		{-3.f, -0.3f, 1.35f},
		{-3.f, 0.f, 1.35f},
		{-2.5f, 0.f, 1.125f},
		{-2.5f, -0.3f, 1.125f},
		{-2.65f, -0.3f, 0.9375f},
		{-2.65f, 0.f, 0.9375f},
		{-2.f, -0.3f, 0.9f},
		{-1.9f, -0.3f, 0.6f},
		{-1.9f, 0.f, 0.6f},
		{1.7f, 0.f, 1.425f},
		{1.7f, -0.66f, 1.425f},
		{1.7f, -0.66f, 0.6f},
		{1.7f, 0.f, 0.6f},
		{2.6f, 0.f, 1.425f},
		{2.6f, -0.66f, 1.425f},
		{3.1f, -0.66f, 0.825f},
		{3.1f, 0.f, 0.825f},
		{2.3f, 0.f, 2.1f},
		{2.3f, -0.25f, 2.1f},
		{2.4f, -0.25f, 2.025f},
		{2.4f, 0.f, 2.025f},
		{2.7f, 0.f, 2.4f},
		{2.7f, -0.25f, 2.4f},
		{3.3f, -0.25f, 2.4f},
		{3.3f, 0.f, 2.4f},
		{2.8f, 0.f, 2.475f},
		{2.8f, -0.25f, 2.475f},
		{3.525f, -0.25f, 2.49375f},
		{3.525f, 0.f, 2.49375f},
		{2.9f, 0.f, 2.475f},
		{2.9f, -0.15f, 2.475f},
		{3.45f, -0.15f, 2.5125f},
		{3.45f, 0.f, 2.5125f},
		{2.8f, 0.f, 2.4f},
		{2.8f, -0.15f, 2.4f},
		{3.2f, -0.15f, 2.4f},
		{3.2f, 0.f, 2.4f},
		{0.f, 0.f, 3.15f},
		{0.8f, 0.f, 3.15f},
		{0.8f, -0.45f, 3.15f},
		{0.45f, -0.8f, 3.15f},
		{0.f, -0.8f, 3.15f},
		{0.f, 0.f, 2.85f},
		{1.4f, 0.f, 2.4f},
		{1.4f, -0.784f, 2.4f},
		{0.784f, -1.4f, 2.4f},
		{0.f, -1.4f, 2.4f},
		{0.4f, 0.f, 2.55f},
		{0.4f, -0.224f, 2.55f},
		{0.224f, -0.4f, 2.55f},
		{0.f, -0.4f, 2.55f},
		{1.3f, 0.f, 2.55f},
		{1.3f, -0.728f, 2.55f},
		{0.728f, -1.3f, 2.55f},
		{0.f, -1.3f, 2.55f},
		{1.3f, 0.f, 2.4f},
		{1.3f, -0.728f, 2.4f},
		{0.728f, -1.3f, 2.4f},
		{0.f, -1.3f, 2.4f},
		{0.f, 0.f, 0.f},
		{1.425f, -0.798f, 0.f},
		{1.5f, 0.f, 0.075f},
		{1.425f, 0.f, 0.f},
		{0.798f, -1.425f, 0.f},
		{0.f, -1.5f, 0.075f},
		{0.f, -1.425f, 0.f},
		{1.5f, -0.84f, 0.075f},
		{0.84f, -1.5f, 0.075f}
	};

	static GLfloat vdata_base[4][3] = {
		{-2.0,-1.0,-2.0}, {-2.0,-1.0,2.0}, {2.0,-1.0,-2.0}, {2.0,-1.0,2.0}
	};

	static GLfloat normaldata_base[4][3] {
		{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}
	};


	
	// ICOSAHEDRON BUFFERS
    glGenBuffers(1, &ico_vertpos_buffer);
    glGenBuffers(1, &ico_index_buffer);
	glGenBuffers(1, &ico_normal_buffer);

	glGenVertexArrays(1, &icoVAO);
	glBindVertexArray(icoVAO);
	
    glBindBuffer(GL_ARRAY_BUFFER, ico_vertpos_buffer);
    glBufferData(GL_ARRAY_BUFFER, 36*sizeof(float), (void *)&(vdata_ico[0][0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, ico_normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(float), (void *)&(vdata_ico[0][0]), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ico_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 60*sizeof(int), (void *)&(tindices[0][0]), GL_STATIC_DRAW);

	// ICOSAHEDRON ATTRIBUTE ASSIGNMENT
    glBindBuffer(GL_ARRAY_BUFFER, ico_vertpos_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	glBindBuffer(GL_ARRAY_BUFFER, ico_normal_buffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	// CUBE BUFFERS
	glGenBuffers(1, &cube_vertpos_buffer);
    glGenBuffers(1, &cube_index_buffer);
	glGenBuffers(1, &cube_normal_buffer);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertpos_buffer);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), (void *)&(vdata_cube), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cube_normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), (void *)&(vdata_cube), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(int), (void *)&(cube_indices), GL_STATIC_DRAW);

	// CUBE ATTRIBUTE ASSIGNMENT
    glBindBuffer(GL_ARRAY_BUFFER, cube_vertpos_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	glBindBuffer(GL_ARRAY_BUFFER, cube_normal_buffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	// glGenBuffers(1, &teapot_controlPoints_buffer);
	// glGenBuffers(1, &teapot_patches_buffer);

	// glGenVertexArrays(1, &teapotVAO);
	// glBindVertexArray(teapotVAO);

	// glBindBuffer(GL_ARRAY_BUFFER, teapot_controlPoints_buffer);
	// glBufferData(GL_ARRAY_BUFFER, (463-337)*sizeof(float), (void*)&(teapot_controlPoints), GL_STATIC_DRAW);

	// glBindBuffer(GL_ARRAY_BUFFER)

	glGenBuffers(1, &vertpos_base_buffer);
	glGenBuffers(1, &normal_base_buffer);
	
	// ground icoVAO/VBO
	glGenVertexArrays(1, &groundVAO);
    glBindVertexArray(groundVAO);

	
    // glGenBuffers(1, &normal_base_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertpos_base_buffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), (void* )&(vdata_base[0][0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertpos_base_buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, normal_base_buffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), (void* )&(normaldata_base[0][0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	glBindVertexArray(0);
	

	std::cout << "Buffers bound\n";

    initShaders();

	TeapotPatch teapot;

    glEnable(GL_DEPTH_TEST);

	float angle = 0;
	
    vec3 cameraPos = vec3(-2.0f, 1.5f, 2.0f);
    mat4 mvp, view, projection, model, mv;
	mat3 normalMtx;

	mvp_location_phong = glGetUniformLocation(phongProgram,"MVP");
	mv_location_phong = glGetUniformLocation(phongProgram, "MV");
	model_location_phong = glGetUniformLocation(phongProgram, "modelMtx");
	normal_location_phong = glGetUniformLocation(phongProgram, "normalMtx");
	viewport_location = glGetUniformLocation(phongProgram, "viewportMatrix");
	cameraPos_location_phong = glGetUniformLocation(phongProgram, "cameraPos");
	lineWidth_location = glGetUniformLocation(phongProgram, "Line.width");
	lineColor_location = glGetUniformLocation(phongProgram, "Line.color");
	tesslevel_location = glGetUniformLocation(phongProgram, "tessLevel");
	
	glClearColor(0.1,0.1,0.1,0);

	float tPrev, rotSpeed, t, deltaT;

	angle = glm::radians(140.0f);

    tPrev = 0;
    rotSpeed = glm::pi<float>()/800.0f;

	GLuint phongSpecPhong = glGetSubroutineIndex(phongProgram, GL_FRAGMENT_SHADER, "specularPhong");
	GLuint blinnSpecPhong = glGetSubroutineIndex(phongProgram, GL_FRAGMENT_SHADER, "specularBlinnPhong");

	glUniform1i(glGetUniformLocation(teapotProgram,"TessLevel"), tessLevel);
    glUniform1f(glGetUniformLocation(teapotProgram,"LineWidth"), 0.8f);
    glUniform4f(glGetUniformLocation(teapotProgram,"LineColor"), 0.05f,0.0f,0.05f,1.0f);
    glUniform4f(glGetUniformLocation(teapotProgram,"LightPosition"), 0.0f,0.0f,0.0f,1.0f);
    glUniform3f(glGetUniformLocation(teapotProgram,"LightIntensity"), 1.0f,1.0f,1.0f);
    glUniform3f(glGetUniformLocation(teapotProgram,"Kd"), 0.9f,0.9f,1.0f);

	glPatchParameteri(GL_PATCH_VERTICES, 16);

    while (!glfwWindowShouldClose(window))
    {
		model = mat4(1.0f);
		
		glUseProgram(phongProgram);

		
		glUniform3fv(cameraPos_location_phong, 1, &(cameraPos)[0]);
		view = glm::lookAt(cameraPos, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));

		glUniform4f(lineColor_location, 1.0f,1.0f,1.0f,1.0f);
		glUniform1f(lineWidth_location, 0.8f);
		
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
    	
    	angle += 0.005f;
		if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

	    cameraPos = vec3(2.0f * cos(angle), 1.5f, 2.0f * sin(angle));
		

    	projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.3f, 100.0f);
		model = glm::rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));
		mv = view * model;
    	mvp = projection * view * model;
		normalMtx = glm::mat3(glm::transpose(glm::inverse(mv)));

		glUniformMatrix4fv(mvp_location_phong, 1, GL_FALSE, &(mvp)[0][0]);
		glUniformMatrix4fv(mv_location_phong, 1, GL_FALSE, &(mv)[0][0]);
		glUniformMatrix4fv(model_location_phong, 1, GL_FALSE, &(model)[0][0]);
		glUniformMatrix4fv(normal_location_phong, 1, GL_FALSE, &(normalMtx)[0][0]);
		glUniformMatrix4fv(viewport_location, 1, GL_FALSE, &(viewport)[0][0]);

		glUniformMatrix4fv(glGetUniformLocation(teapotProgram,"ModelViewMatrix"), 1, GL_FALSE, &(mv)[0][0]);
    	glUniformMatrix3fv(glGetUniformLocation(teapotProgram,"NormalMatrix"), 1, GL_FALSE, &(normalMtx)[0][0]);
    	glUniformMatrix4fv(glGetUniformLocation(teapotProgram,"MVP"), 1, GL_FALSE, &(mvp)[0][0]);
    	glUniformMatrix4fv(glGetUniformLocation(teapotProgram,"ViewportMatrix"), 1, GL_FALSE, &(viewport)[0][0]);

        glUniform1i(glGetUniformLocation(teapotProgram,"TessLevel"), tessLevel);

		if(usePhongSpec) {
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &phongSpecPhong);
		} else {
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &blinnSpecPhong);
		}

		glBindVertexArray(groundVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		switch(modelToRender){
			case 1: 
				glBindVertexArray(icoVAO);
				glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT,0);
				break;
			case 2:
				glBindVertexArray(cubeVAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);
			case 3:
				glUseProgram(teapotProgram);
				teapot.render();
				break;
			default: break;
		}
	    
    	glBindVertexArray(0);

		showFPS(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
