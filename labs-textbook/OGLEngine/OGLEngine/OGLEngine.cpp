#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/noise.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include <OGLEngine.hpp>
#include "teapotpatch.h"
#include "teapotdata.h"

// the X and Z values are for drawing an icosahedron
#define IX 0.525731112119133606 
#define IZ 0.850650808352039932
#define M_PI = 3.14159265

GLFWwindow* window;

GLuint phongProgram;
GLuint teapotProgram;

GLuint icoVAO;
GLuint cubeVAO;
GLuint teapotVAO;
GLuint groundVAO;

bool usePhongSpec = true;

int lastTime = 0;
int nFrames = 0;
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

OGLEngine::OGLEngine(int OPENGL_MAJOR, int OPENGL_MINOR, int WINDOW_WIDTH, int WINDOW_HEIGHT, const char* WINDOW_NAME) {
	for(auto& _key : _keys) _key = GL_FALSE;

	_mousePos = glm::vec2(MOUSE_UNINIT, MOUSE_UNINIT);
	_leftMouseButtonState = GLFW_RELEASE;
	_selectedCam = 1;
}

OGLEngine::~OGLEngine() {
	
}

void OGLEngine::_setupGLFW() {
	glfwSetErrorCallback(error_callback);
	// initialize GLFW
    if( !glfwInit() ) {
        fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
        _errorCode = OPENGL_ENGINE_ERROR_GLFW_INIT;
    } else {
	if(DEBUG) fprintf( stdout, "[INFO]: GLFW %d.%d.%d initialized\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, _majorVersion );	        // request OpenGL vX.
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, _minorVersion );	        // request OpenGL v.X
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );                  // request forward compatible OpenGL context
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );        // request OpenGL Core Profile context
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );                             // request double buffering
	glfwWindowHint( GLFW_RESIZABLE, _windowResizable );		                // set if our window should be able to be resized
	
	// create a window for a given size, with a given title
	_window = glfwCreateWindow( _windowWidth, _windowHeight, _windowTitle, nullptr, nullptr );
	if( !_window ) {						                                // if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		_errorCode = OPENGL_ENGINE_ERROR_GLFW_WINDOW;
	} else {
		if(DEBUG) fprintf( stdout, "[INFO]: GLFW Window created\n" );
		glfwMakeContextCurrent(_window);		                                // make the created window the current window
		glfwSwapInterval(1);				                            // update our screen after at least 1 screen refresh

		glfwSetWindowUserPointer(_window, (void*)this);
		glfwSetWindowSizeCallback(_window, _windowResizeCallback);
	}

	glfwSetKeyCallback(_window, key_callback);
	glfwSetMouseButtonCallback(_window, mouse_button_callback);
	glfwSetCursorPosCallback(_window, cursor_pos_callback);
	glfwSetScrollCallback(_window, scroll_callback);
	}
}

void OGLEngine::_setupOGL() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

int storeTex( GLubyte * data, int w, int h ) {
	GLuint texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return texID;
}

int generate2DTex(float baseFreq = 4.0f, float persistence = 0.5f, int w = 128, int h = 128, bool periodic = false) {

	int width = w;
	int height = h;

	printf("Generating noise texture...");

	GLubyte *data = new GLubyte[ width * height * 4 ];

	float xFactor = 1.0f / (width - 1);
	float yFactor = 1.0f / (height - 1);

	for( int row = 0; row < height; row++ ) {
		for( int col = 0 ; col < width; col++ ) {
			float x = xFactor * col;
			float y = yFactor * row;
			float sum = 0.0f;
			float freq = baseFreq;
			float persist = persistence;
			for( int oct = 0; oct < 4; oct++ ) {
				glm::vec2 p(x * freq, y * freq);

				float val = 0.0f;
				if (periodic) {
					val = glm::perlin(p, glm::vec2(freq)) * persist;
				} else {
					val = glm::perlin(p) * persist;
				}

				sum += val;

				float result = (sum + 1.0f) / 2.0f;

				// Clamp strictly between 0 and 1
				result = result > 1.0f ? 1.0f : result;
				result = result < 0.0f ? 0.0f : result;

				// Store in texture
				data[((row * width + col) * 4) + oct] = (GLubyte) ( result * 255.0f );
				freq *= 2.0f;
				persist *= persistence;
			}
		}
	}

	int texID = storeTex(data, width, height);
	delete [] data;

	printf("done.\n");
	return texID;
}

int generatePeriodic2DTex(float baseFreq = 4.0f, float persist = 0.5f, int w = 128, int h = 128) {
	return generate2DTex(baseFreq, persist, w, h, true);
}

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

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if(abs(yoffset) > 0) {
		// something goes here
	}
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
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

void initWindow() {
	glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    window = glfwCreateWindow(1200, 600, "A2", NULL, NULL);
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
}

void OGLEngine::_setupGLFW() {

}

void OGLEngine::_setupOGL() {

}

void OGLEngine::_setupShaders()
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
	teapotProgram = glCreateProgram();

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
	glLinkProgram(teapotProgram); 

	glUseProgram(phongProgram);
	
}

void OGLEngine::_setupBuffers() {

	GLuint ico_vertpos_buffer;
	GLuint ico_normal_buffer;
    GLuint ico_index_buffer;

	GLuint cube_vertpos_buffer;
	GLuint cube_normal_buffer;
	GLuint cube_index_buffer;

	GLuint vertpos_base_buffer;
	GLuint normal_base_buffer;
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

	GLuint noiseTex = generatePeriodic2DTex();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noiseTex);

	glBindVertexArray(0);
}

void showFPS(GLFWwindow* window) {
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
	    char ss[500] = {};
		std::string wTitle = "A2";
        nFrames++;
        if ( delta >= 1.0 ){ // If last update was more than 1 sec ago
            double fps = ((double)(nFrames)) / delta;
            sprintf(ss,"%s running at %lf FPS.",wTitle.c_str(),fps);
            glfwSetWindowTitle(window, ss);
            nFrames = 0;
            lastTime = currentTime;
        }
}
    
void OGLEngine::run()
{	
	GLint mvp_location_phong, model_location_phong, mv_location_phong;
	GLint normal_location_phong;
	GLint viewport_location;
	GLint cameraPos_location_phong;
	GLint lineWidth_location, lineColor_location;

	initWindow();
	_setupBuffers();
    _setupShaders();

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
	
	glClearColor(0.1,0.1,0.1,0);

	float tPrev, rotSpeed, t, deltaT;

	angle = glm::radians(140.0f);

    tPrev = 0;
    rotSpeed = glm::pi<float>()/800.0f;

	GLuint phongSpecPhong = glGetSubroutineIndex(phongProgram, GL_FRAGMENT_SHADER, "specularPhong");
	GLuint blinnSpecPhong = glGetSubroutineIndex(phongProgram, GL_FRAGMENT_SHADER, "specularBlinnPhong");

	glUniform1i(glGetUniformLocation(teapotProgram,"TessLevel"), tessLevel);
    glUniform1f(glGetUniformLocation(teapotProgram,"LineWidth"), 0.8f);
    glUniform4f(glGetUniformLocation(teapotProgram,"LineColor"), 1.0f,1.0f,1.0f,1.0f);
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

		glm::mat3 nm = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));

		

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
				break;
			case 3:
				glUseProgram(teapotProgram);
				model = glm::scale(model, vec3(0.25, 0.25, 0.25));
				model = glm::rotate(model, glm::radians(-90.0f), vec3(1,0,0));
				mv = view * model;
				nm = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
				mvp = projection * view * model;
				glUniformMatrix4fv(glGetUniformLocation(teapotProgram,"ModelViewMatrix"), 1, GL_FALSE, &(mv)[0][0]);
				glUniformMatrix3fv(glGetUniformLocation(teapotProgram,"NormalMatrix"), 1, GL_FALSE, &(nm)[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(teapotProgram,"MVP"), 1, GL_FALSE, &(mvp)[0][0]);
				glUniformMatrix4fv(glGetUniformLocation(teapotProgram,"ViewportMatrix"), 1, GL_FALSE, &(viewport)[0][0]);

				glUniform1i(glGetUniformLocation(teapotProgram,"TessLevel"), tessLevel);
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
