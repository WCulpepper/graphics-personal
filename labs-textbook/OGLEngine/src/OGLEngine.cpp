#include "OGLEngine.hpp"

// the X and Z values are for drawing an icosahedron
#define IX 0.525731112119133606 
#define IZ 0.850650808352039932

#define M_PI 3.14159265358979323846

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint icoVAO;
GLuint cubeVAO;
GLuint teapotVAO;
GLuint groundVAO;


bool usePhongSpec = true;
bool showWireFrame = false;

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

OGLEngine::OGLEngine(int majorVersion, int minorVersion, int windowWidth, int windowHeight, const char* windowName) {
	fprintf( stdout, "\n[INFO]: currently in constructor\n" );
	DEBUG = true;
	for(auto& _key : _keys) _key = GL_FALSE;

	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_majorVersion = majorVersion;
	_minorVersion = minorVersion;
	
	_windowTitle = (char*)malloc(sizeof(char)*strlen(windowName));
	strcpy(_windowTitle, windowName);

	_window = nullptr;
	_mousePos = glm::vec2(MOUSE_UNINIT, MOUSE_UNINIT);
	_leftMouseButtonState = GLFW_RELEASE;
	_selectedCam = 1;

	_errorCode = OPENGL_ENGINE_ERROR_NO_ERROR;
	_isInitialized = false;
	_isCleanedUp = false;
}

OGLEngine::OGLEngine() {
	std::cout <<"Default constructor\n";
}

OGLEngine::~OGLEngine() {
	shutdown();
	free(_windowTitle);
}

void OGLEngine::keyEventHandler(GLint key, GLint action) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		modelToRender = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		modelToRender = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		modelToRender = 3;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		modelToRender = 4;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		modelToRender = 5;
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		modelToRender = 6;
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
		modelToRender = 7;
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
		modelToRender = 8;
	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
		usePhongSpec = true;
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		usePhongSpec = false;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		showWireFrame = !showWireFrame;
}

void OGLEngine::mbEventHandler(GLint button, GLint action) {
	if(button == GLFW_MOUSE_BUTTON_LEFT) {
		_leftMouseButtonState = action;
	} 
}

void OGLEngine::cursorPosEventHandler(glm::vec2 currMousePosition) {
	if(_mousePos.x == MOUSE_UNINIT) {
		_mousePos = currMousePosition;
	}

	if(_leftMouseButtonState == GLFW_PRESS) {
		_arcballCam->rotate((currMousePosition.x - _mousePos.x) * 0.005f,
                            (currMousePosition.y - _mousePos.y) * 0.005f );
	}

	_mousePos = currMousePosition;
}

void OGLEngine::scrollWheelEventHandler(double offset) {
	if(abs(offset) > 0) {
		_arcballCam->moveForward(offset);
	}
}

void OGLEngine::windowResizeHandler(int width, int height) {
	_windowWidth = width;
	_windowHeight = height;
}

void OGLEngine::initialize() {
	if(!_isInitialized) {
		std::cout << DEBUG << std::endl;
		_setupGLFW();
		_setupOGL();

		//if (DEBUG) printOGLInfo();

		_setupBuffers();
		_setupFBO();
		_setupTextures();
		_setupShaders();
		_setupScene();

		_isInitialized = true;
	}
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
		fprintf(stdout, "[INFO]: Creating window...\n");
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
			glfwSetWindowSizeCallback(_window, window_resize_callback);
		}

		glfwSetKeyCallback(_window, key_callback);
		glfwSetMouseButtonCallback(_window, mouse_button_callback);
		glfwSetCursorPosCallback(_window, cursor_pos_callback);
		glfwSetScrollCallback(_window, scroll_callback);
	}
}

void OGLEngine::_setupOGL() {
	fprintf(stdout, "[INFO]: Setting up OpenGL...\n");
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void OGLEngine::_setupShaders()
{
	char *vsSource_wireframe, *fsSource_wireframe, *gsSource_wireframe, 
	*vsSource_teapot, *fsSource_teapot, *gsSource_teapot, *tcsSource_teapot, *tesSource_teapot;

	char *vsSource_rt, *fsSource_rt;

	GLuint vsWireframe, gsWireframe, fsWireframe, 
	vsTeapot, gsTeapot, fsTeapot, tcsTeapot, tesTeapot;

	GLuint vsRT, fsRT;

	vsSource_wireframe = (char *)malloc(sizeof(char)*20000);
	gsSource_wireframe = (char *)malloc(sizeof(char)*20000);
	fsSource_wireframe = (char *)malloc(sizeof(char)*20000);

	vsSource_teapot = (char *)malloc(sizeof(char)*20000);
	gsSource_teapot = (char *)malloc(sizeof(char)*20000);
	fsSource_teapot = (char *)malloc(sizeof(char)*20000);
	tcsSource_teapot = (char *)malloc(sizeof(char)*20000);
	tesSource_teapot = (char *)malloc(sizeof(char)*20000);

	fsSource_rt = (char *)malloc(sizeof(char)*20000);
	vsSource_rt = (char *)malloc(sizeof(char)*20000);

	vsSource_wireframe[0]='\0';
	gsSource_wireframe[0]='\0';
	fsSource_wireframe[0]='\0';

	vsSource_teapot[0]='\0';
	gsSource_teapot[0]='\0';
	fsSource_teapot[0]='\0';
	tcsSource_teapot[0]='\0';
	tesSource_teapot[0]='\0';

	vsSource_rt[0]='\0';
	fsSource_rt[0]='\0';

	_wireframeProgram = glCreateProgram();
	_teapotProgram = glCreateProgram();
	_rtProgram = glCreateProgram();

	readShader("shader/wireframe_vertex.vs",vsSource_wireframe);
	readShader("shader/wireframe_fragment.fs",fsSource_wireframe);
	readShader("shader/wireframe_geometry.gs",gsSource_wireframe);

	readShader("shader/teapot_vertex.vs",vsSource_teapot);
	readShader("shader/teapot_fragment.fs",fsSource_teapot);
	readShader("shader/teapot_geometry.gs",gsSource_teapot);
	readShader("shader/teapot.tcs",tcsSource_teapot);
	readShader("shader/teapot.tes",tesSource_teapot);

	readShader("shader/rt.vs", vsSource_rt);
	readShader("shader/rt.fs", fsSource_rt);

	vsWireframe = loadShader(vsSource_wireframe,GL_VERTEX_SHADER);
	fsWireframe = loadShader(fsSource_wireframe,GL_FRAGMENT_SHADER);
	gsWireframe = loadShader(gsSource_wireframe,GL_GEOMETRY_SHADER);

	vsTeapot = loadShader(vsSource_teapot,GL_VERTEX_SHADER);
	fsTeapot = loadShader(fsSource_teapot,GL_FRAGMENT_SHADER);
	gsTeapot = loadShader(gsSource_teapot,GL_GEOMETRY_SHADER);
	tcsTeapot = loadShader(tcsSource_teapot, GL_TESS_CONTROL_SHADER);
	tesTeapot = loadShader(tesSource_teapot, GL_TESS_EVALUATION_SHADER);

	vsRT = loadShader(vsSource_rt, GL_VERTEX_SHADER);
	fsRT = loadShader(fsSource_rt, GL_FRAGMENT_SHADER);

	glAttachShader(_wireframeProgram,vsWireframe);
	glAttachShader(_wireframeProgram,fsWireframe);
	glAttachShader(_wireframeProgram,gsWireframe);

	glAttachShader(_teapotProgram,vsTeapot);
	glAttachShader(_teapotProgram,fsTeapot);
	glAttachShader(_teapotProgram,gsTeapot);
	glAttachShader(_teapotProgram,tcsTeapot);
	glAttachShader(_teapotProgram,tesTeapot);

	glAttachShader(_rtProgram, vsRT);
	glAttachShader(_rtProgram, fsRT);

	glLinkProgram(_wireframeProgram);
	glLinkProgram(_teapotProgram); 
	glLinkProgram(_rtProgram);

	glUseProgram(_wireframeProgram);

	_wfUniformLocations.mvpMtx = glGetUniformLocation(_wireframeProgram,"MVP");
	_wfUniformLocations.mvMtx = glGetUniformLocation(_wireframeProgram, "MV");
	_wfUniformLocations.modelMtx = glGetUniformLocation(_wireframeProgram, "modelMtx");
	_wfUniformLocations.normalMtx = glGetUniformLocation(_wireframeProgram, "normalMtx");
	_wfUniformLocations.viewportMtx = glGetUniformLocation(_wireframeProgram, "viewportMatrix");
	_wfUniformLocations.cameraPos = glGetUniformLocation(_wireframeProgram, "cameraPos");
	_wfUniformLocations.lightColor = glGetUniformLocation(_wireframeProgram, "lightColor");
	_wfUniformLocations.lightPos = glGetUniformLocation(_wireframeProgram, "lightPos");
	_wfUniformLocations.lineWidth = glGetUniformLocation(_wireframeProgram, "Line.width");
	_wfUniformLocations.lineColor = glGetUniformLocation(_wireframeProgram, "Line.color");
	_wfUniformLocations.materialIndex = glGetUniformBlockIndex(_wireframeProgram, "MaterialSettings");
	_wfUniformLocations.wfBoolLocation = glGetUniformLocation(_wireframeProgram, "showWireFrame");

	_wfSubroutineLocations.phongSpec = glGetSubroutineIndex(_wireframeProgram, GL_FRAGMENT_SHADER, "specularPhong");
	_wfSubroutineLocations.blinnPhongSpec = glGetSubroutineIndex(_wireframeProgram, GL_FRAGMENT_SHADER, "specularBlinnPhong");

	_tessUniformLocations.mvMtx = glGetUniformLocation(_teapotProgram,"ModelViewMatrix");
	_tessUniformLocations.normalMtx = glGetUniformLocation(_teapotProgram,"NormalMatrix");
	_tessUniformLocations.mvpMtx = glGetUniformLocation(_teapotProgram,"MVP");
	_tessUniformLocations.viewportMtx = glGetUniformLocation(_teapotProgram,"ViewportMatrix");
	_tessUniformLocations.tessLevel = glGetUniformLocation(_teapotProgram,"TessLevel");

	_tessUniformLocations.lineWidth = glGetUniformLocation(_teapotProgram,"LineWidth");
	_tessUniformLocations.lineColor = glGetUniformLocation(_teapotProgram,"LineColor");
	_tessUniformLocations.lightIntensity = glGetUniformLocation(_teapotProgram,"LightIntensity");
	_tessUniformLocations.lightPos = glGetUniformLocation(_teapotProgram,"LightPosition");
	_tessUniformLocations.kd = glGetUniformLocation(_teapotProgram,"Kd");

	_rtUniformLocations.projectionMtx = glGetUniformLocation(_rtProgram, "projectionMtx");
	_rtUniformLocations.viewMtx = glGetUniformLocation(_rtProgram, "viewMtx");
	_rtUniformLocations.cameraPos = glGetUniformLocation(_rtProgram, "camPos");
	_rtUniformLocations.cameraUp = glGetUniformLocation(_rtProgram, "camUp");
	_rtUniformLocations.cameraGaze = glGetUniformLocation(_rtProgram, "camGaze");	
	_rtUniformLocations.mvpMtx = glGetUniformLocation(_rtProgram, "mvpMtx");
	_rtUniformLocations.normalMtx = glGetUniformLocation(_rtProgram, "normalMtx");

	glUniform4f(_wfUniformLocations.lineColor, 1.0f,1.0f,1.0f,1.0f);
	glUniform1f(_wfUniformLocations.lineWidth, 0.8f);
	glUniform1f(_tessUniformLocations.lineWidth, 0.8f);
    glUniform4f(_tessUniformLocations.lineColor, 1.0f,1.0f,1.0f,1.0f);
    glUniform4f(_tessUniformLocations.lightPos, 3.0f,3.0f,3.0f,1.0f);
    glUniform3f(_tessUniformLocations.lightIntensity, 1.0f,1.0f,1.0f);
    glUniform3f(_tessUniformLocations.kd, 0.9f,0.9f,1.0f);

	glUniform3fv(_wfUniformLocations.lightColor, 1, &(_lightColor)[0]);
	glUniform3fv(_wfUniformLocations.lightPos, 1, &(_lightPos)[0]);

	_firstPassFuncIndex = glGetSubroutineIndex(_wireframeProgram, GL_FRAGMENT_SHADER, "pass1");
	_secondPassFuncIndex = glGetSubroutineIndex(_wireframeProgram, GL_FRAGMENT_SHADER, "pass2");
	
}

void OGLEngine::_setupBuffers() {
	fprintf(stdout, "[INFO]: Setting up buffers...\n");
	GLuint ico_vertpos_buffer;
	GLuint ico_normal_buffer;
    GLuint ico_index_buffer;

	GLuint cube_vertpos_buffer;
	GLuint cube_normal_buffer;
	GLuint cube_index_buffer;

	GLuint vertpos_base_buffer;
	GLuint normal_base_buffer;

	// non-interpolated vertex data

	GLfloat positionData[] = {
        -1, -1,
        1, -1, 
        1, 1, 
        -1, 1
    };

    GLfloat texCoord[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1 
    }; 

    GLuint indexData[] = {
        0, 1, 2, 
        3, 0, 2
    }; 
	

	struct IcoSSBOData {
		vec4 vdata_ico[12] = { vec4(-IX, 0.0, IZ, 1.0), vec4(IX, 0.0, IZ, 1.0), vec4(-IX, 0.0, -IZ, 1.0), vec4(IX, 0.0, -IZ, 1.0),    
							vec4(0.0, IZ, IX, 1.0), vec4(0.0, IZ, -IX, 1.0), vec4(0.0, -IZ, IX, 1.0), vec4(0.0, -IZ, -IX, 1.0),    
							vec4(IZ, IX, 0.0, 1.0), vec4(-IZ, IX, 0.0, 1.0), vec4(IZ, -IX, 0.0, 1.0), vec4(-IZ, -IX, 0.0, 1.0) 
		};

		vec4 indices[20] = { vec4(0,4,1, 1), vec4(0,9,4, 1), vec4(9,5,4, 1), vec4(4,5,8, 1), vec4(4,8,1, 1),    
							vec4(8,10,1, 1), vec4(8,3,10, 1), vec4(5,3,8, 1), vec4(5,2,3, 1), vec4(2,7,3, 1),    
							vec4(7,10,3, 1), vec4(7,6,10, 1), vec4(7,11,6, 1), vec4(11,0,6, 1), vec4(0,1,6, 1), 
							vec4(6,1,10, 1), vec4(9,0,11, 1), vec4(9,11,2, 1), vec4(9,2,5, 1), vec4(7,2,11, 1)  };
	} icoSSBOData;

	vec4 vdata_icoSSBO[12] = { vec4(-IX, 0.0, IZ, 1.0), vec4(IX, 0.0, IZ, 1.0), vec4(-IX, 0.0, -IZ, 1.0), vec4(IX, 0.0, -IZ, 1.0),    
							vec4(0.0, IZ, IX, 1.0), vec4(0.0, IZ, -IX, 1.0), vec4(0.0, -IZ, IX, 1.0), vec4(0.0, -IZ, -IX, 1.0),    
							vec4(IZ, IX, 0.0, 1.0), vec4(-IZ, IX, 0.0, 1.0), vec4(IZ, -IX, 0.0, 1.0), vec4(-IZ, -IX, 0.0, 1.0) 
	};

	vec4 ndata_icoSSBO[12] = { normalize(vec4(-IX, 0.0, IZ, 1.0)), normalize(vec4(IX, 0.0, IZ, 1.0)), normalize(vec4(-IX, 0.0, -IZ, 1.0)), normalize(vec4(IX, 0.0, -IZ, 1.0)),    
							normalize(vec4(0.0, IZ, IX, 1.0)), normalize(vec4(0.0, IZ, -IX, 1.0)), normalize(vec4(0.0, -IZ, IX, 1.0)), normalize(vec4(0.0, -IZ, -IX, 1.0)),    
							normalize(vec4(IZ, IX, 0.0, 1.0)), normalize(vec4(-IZ, IX, 0.0, 1.0)), normalize(vec4(IZ, -IX, 0.0, 1.0)), normalize(vec4(-IZ, -IX, 0.0, 1.0)) 
		};

	vec4 indices_icoSSBO[20] = { vec4(0,4,1, 1), vec4(0,9,4, 1), vec4(9,5,4, 1), vec4(4,5,8, 1), vec4(4,8,1, 1),    
							vec4(8,10,1, 1), vec4(8,3,10, 1), vec4(5,3,8, 1), vec4(5,2,3, 1), vec4(2,7,3, 1),    
							vec4(7,10,3, 1), vec4(7,6,10, 1), vec4(7,11,6, 1), vec4(11,0,6, 1), vec4(0,1,6, 1), 
							vec4(6,1,10, 1), vec4(9,0,11, 1), vec4(9,11,2, 1), vec4(9,2,5, 1), vec4(7,2,11, 1)  };



	struct CubeSSBOData {
		vec4 vdata_cube[8] = { vec4(-0.5, -0.5, -0.5, 1.0),
            vec4(0.5, -0.5, -0.5, 1.0),
            vec4(0.5,  0.5, -0.5, 1.0),
            vec4(-0.5, 0.5, -0.5, 1.0),
            vec4(-0.5, -0.5, 0.5, 1.0), 
            vec4(0.5, -0.5, 0.5, 1.0),
            vec4(0.5, 0.5, 0.5, 1.0),
            vec4(-0.5, 0.5, 0.5, 1.0) };
		
		vec4 indices[12] = { vec4(0, 2, 1, 1), vec4(0, 3, 2, 1),
            vec4(1, 2, 5, 1), vec4(5, 2, 6, 1), 
            vec4(2, 7, 6, 1), vec4(3, 7, 2, 1), 
            vec4(0, 1, 4, 1), vec4(1, 5, 4, 1), 
            vec4(4, 5, 6, 1), vec4(4, 6, 7, 1), 
            vec4(0, 4, 3, 1), vec4(4, 7, 3, 1) };
	} cubeSSBOData;

	// These are the 12 vertices for the icosahedron, interpolated with texture coordinates
	Vertex icoVerts[12] = {
		{-IX, 0.0, IZ, -IX, 0.0, IZ, 0.0, 0.0}, 
		{IX, 0.0, IZ, IX, 0.0, IZ, 0.0, 0.0}, 
		{-IX, 0.0, -IZ, -IX, 0.0, -IZ, 0.0, 0.0}, 
		{IX, 0.0, -IZ, IX, 0.0, -IZ, 0.0, 0.0}, 
		{0.0, IZ, IX, 0.0, IZ, IX, 0.0, 0.0}, 
		{0.0, IZ, -IX, 0.0, IZ, -IX, 0.0, 0.0}, 
		{0.0, -IZ, IX, 0.0, -IZ, IX, 0.0, 0.0}, 
		{0.0, -IZ, -IX, 0.0, -IZ, -IX, 0.0, 0.0}, 
		{IZ, IX, 0.0, IZ, IX, 0.0, 0.0, 0.0}, 
		{-IZ, IX, 0.0, -IZ, IX, 0.0, 0.0, 0.0}, 
		{IZ, -IX, 0.0, IZ, -IX, 0.0, 0.0, 0.0}, 
		{-IZ, -IX, 0.0, -IZ, -IX, 0.0, 0.0, 0.0}
	};

	// These are the 20 faces.  Each of the three entries for each 
	// vertex gives the 3 vertices that make the face.
	static GLint tindices[20][3] = { 
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
		{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
		{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} 
	};

	Texture t = {0, "noise"};

	// Mesh icosahedron = Mesh(*icoVerts, **tindices);

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

	
	GLfloat vdata_base[4][3] = {
		{-2.0,-1.0,-2.0}, {-2.0,-1.0,2.0}, {2.0,-1.0,-2.0}, {2.0,-1.0,2.0}
	};

	static GLfloat normaldata_base[4][3] {
		{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}
	};

	
	// ICOSAHEDRON BUFFERS
	GLuint icoVBODs[2];
	glGenBuffers(2, icoVBODs);
    // glGenBuffers(1, &ico_vertpos_buffer);
    glGenBuffers(1, &ico_index_buffer);
	// glGenBuffers(1, &ico_normal_buffer);

	glGenVertexArrays(1, &icoVAO);
	glBindVertexArray(icoVAO);

	glBindBuffer(GL_ARRAY_BUFFER, icoVBODs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(icoVerts), icoVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0 );

	glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(GLfloat)) );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ico_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 60*sizeof(int), (void *)&(tindices[0][0]), GL_STATIC_DRAW);

	GLuint vao, vertexAttribBuffer, indexBuffer;
	glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // texCoord

    // generate a simple quad 2D with two triangles
    // 4 vertices
    // 6 indices for the 2 triangles in total

	glGenBuffers(1, &vertexAttribBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8*sizeof(GLfloat), positionData);
	glBufferSubData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), 8*sizeof(GLfloat), texCoord);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint), indexData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); // vertex position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8*sizeof(GLfloat))); // texCoord



	GLuint icoSSBO;
	glGenBuffers(1, &icoSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, icoSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(icoSSBOData), &icoSSBOData, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, icoSSBO);

	GLuint icoVertexSSBO;
	glGenBuffers(1, &icoVertexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, icoVertexSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vdata_icoSSBO), &vdata_icoSSBO, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, icoVertexSSBO);

	GLuint icoNormalSSBO;
	glGenBuffers(1, &icoNormalSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, icoNormalSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ndata_icoSSBO), &ndata_icoSSBO,GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, icoNormalSSBO);

	GLuint icoIndexSSBO;
	glGenBuffers(1, &icoIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, icoIndexSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(indices_icoSSBO), &indices_icoSSBO, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, icoIndexSSBO);

	GLuint cubeSSBO;
	glGenBuffers(1, &cubeSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cubeSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(cubeSSBOData), &cubeSSBOData, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, cubeSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

	objects.teapot = new TeapotPatch();
	objects.cube = new Cube();
	objects.ico = new Icosahedron();
	objects.sphere = new Sphere(1.0, 32, 32);

	glBindVertexArray(0);
}

void OGLEngine::_createBufferTexture(GLenum texType, GLenum format, GLuint &texHandle) {
	glActiveTexture(texType);
	glGenTextures(1, &texHandle);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	glTexStorage2D(GL_TEXTURE_2D, 1, format, _windowWidth, _windowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void OGLEngine::_setupFBO() {
	// create FBO
	glGenFramebuffers(1, &_fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboHandle);

	// create depth buffers + render textures
	glGenRenderbuffers(1, &_depthBufferHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthBufferHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _windowWidth, _windowHeight);

	_createBufferTexture(GL_TEXTURE0, GL_RGB32F, _positionBufferHandle);
	_createBufferTexture(GL_TEXTURE1, GL_RGB32F, _normalBufferHandle);
	_createBufferTexture(GL_TEXTURE2, GL_RGB8, _colorBufferHandle);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBufferHandle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionBufferHandle, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalBufferHandle, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _colorBufferHandle, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBufferHandle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionBufferHandle, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalBufferHandle, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _colorBufferHandle, 0);

	GLenum drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(4, drawBuffers);
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(result == GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer complete\n");
	else
		printf("Framebuffer not complete\n");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLEngine::_setupTextures() {
	return;
}

void OGLEngine::_setupScene() {

	// sets up cameras
	_arcballCam = new ArcballCam();
	_arcballCam->setLookAtPoint(glm::vec3(0.0f, 0.0f, 0.0f));
	_arcballCam->setTheta(glm::radians(90.0f));
	_arcballCam->setPhi(glm::radians(120.0f));
	_arcballCam->setRadius(4);
	_arcballCam->recomputeOrientation();

	_freeCam = new FreeCam();
	_freeCam->setLookAtPoint(glm::vec3(60, 30, 20));
    _freeCam->setTheta( -M_PI / 3.0f );
    _freeCam->setPhi( M_PI / 2.8f );
    _freeCam->recomputeOrientation();
    _freeCamSpeed = glm::vec2(0.25f, 0.04f);

	_cameraPos = _arcballCam->getPosition();
	_cameraAngle = glm::radians(140.0f);
    _rotSpeed = glm::pi<float>()/800.0f;
	_deltaT = 0;
	_tPrev = 0;

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

int generate2DTex(float baseFreq, float persistence, int w, int h, bool periodic) {

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

int generatePeriodic2DTex(float baseFreq, float persist, int w, int h) {
	return generate2DTex(baseFreq, persist, w, h, true);
}

void readShader(const char* fname, char *source) {
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

unsigned int loadShader(const char *source, unsigned int mode) {
	GLuint id;
	id = glCreateShader(mode);

	glShaderSource(id,1,&source,NULL);

	glCompileShader(id);

	char error[1000];

	glGetShaderInfoLog(id,1000,NULL,error);
	printf("Compile status: \n %s\n",error);

	return id;
}

void OGLEngine::_showFPS(GLFWwindow* window) {
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
	    char ss[500] = {};
        nFrames++;
        if ( delta >= 1.0 ){ // If last update was more than 1 sec ago
            double fps = ((double)(nFrames)) / delta;
            sprintf(ss,"%s running at %lf FPS.",_windowTitle,fps);
            glfwSetWindowTitle(window, ss);
            nFrames = 0;
            lastTime = currentTime;
        }
}

void OGLEngine::_setMaterial(Materials::Material m) {
	GLint materialBlockSize;
	glGetActiveUniformBlockiv(_wireframeProgram, _wfUniformLocations.materialIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &materialBlockSize);
	GLubyte *materialBlockBuffer;
	materialBlockBuffer = (GLubyte*)malloc(materialBlockSize);

	const GLchar *materialPropertyNames[] = {"materialDiffuse", "materialSpecular", "materialShininess", "materialAmbient"};
	GLuint materialIndices[4];
	glGetUniformIndices(_wireframeProgram, 4, materialPropertyNames, materialIndices);
	GLint offset[4];
	glGetActiveUniformsiv(_wireframeProgram, 4, materialIndices, GL_UNIFORM_OFFSET, offset);

	memcpy(materialBlockBuffer + offset[0], m.diffuse, sizeof(float[4]));
	memcpy(materialBlockBuffer + offset[1], m.specular, sizeof(float[4]));
	memcpy(materialBlockBuffer + offset[2], &m.shininess, sizeof(float));
	memcpy(materialBlockBuffer + offset[3], m.ambient, sizeof(float[4]));

	GLuint materialUBOHandle;
	glGenBuffers(1, &materialUBOHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, materialUBOHandle);
	glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, materialBlockBuffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, materialUBOHandle);
}

void OGLEngine::_computeAndSendTransformationMatrices(GLuint* shaderProgramHandle,
                                                     glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix,
                                                     GLint mvpMtxLocation, GLint modelMtxLocation, GLint normalMtxLocation) {
	if(shaderProgramHandle) {
		glm::mat4 mvpMatrix = projectionMatrix*viewMatrix*modelMatrix;
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
		glUniformMatrix4fv(mvpMtxLocation, 1, GL_FALSE, &(mvpMatrix)[0][0]);
		glUniformMatrix4fv(modelMtxLocation, 1, GL_FALSE, &(modelMatrix)[0][0]);
		glUniformMatrix3fv(normalMtxLocation, 1, GL_FALSE, &(normalMatrix)[0][0]);
	}
}

void OGLEngine::_computeAndSendWireframeMatrices(GLuint* shaderProgramHandle, glm::mat4 modelMatrix, glm::mat4 viewMatrix, 
                                                    glm::mat4 viewportMatrix, GLint viewportMatrixLocation, GLint mvMatrixLocation) {
	if(shaderProgramHandle) {
		glm::mat4 mvMatrix = viewMatrix*modelMatrix;
		glUniformMatrix4fv(mvMatrixLocation, 1, GL_FALSE, &(mvMatrix)[0][0]);
		glUniformMatrix4fv(viewportMatrixLocation, 1, GL_FALSE, &(viewportMatrix)[0][0]);
	}
}

void OGLEngine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx, glm::mat4 viewportMtx) {
	if(!_wireframeProgram || !_teapotProgram || !_rtProgram) {
		std::cout << "Failed to load shader programs!\n"; 
		return;
	}

	glUniformMatrix4fv(_rtUniformLocations.projectionMtx, 1, GL_FALSE, &(projMtx)[0][0]);
	glUniformMatrix4fv(_rtUniformLocations.viewMtx, 1, GL_FALSE, &(viewMtx)[0][0]);

	glm::mat4 model, mv, mvp;
	glm::mat3 normalMtx, nm;

	glUniform1i(_tessUniformLocations.tessLevel, tessLevel);
    

	model = mat4(1.0f);
	// model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0,0.0,0.0));
	

	// model = glm::rotate(model, _cameraAngle, vec3(0.0f, 1.0f, 0.0f));
	// mv = viewMtx * model;
	// mvp = projMtx * viewMtx * model;
	// normalMtx = glm::mat3(glm::transpose(glm::inverse(mv)));
	// glm::mat3 nm = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));

	// glUniformMatrix4fv(_wfUniformLocations.mvpMtx, 1, GL_FALSE, &(mvp)[0][0]);
	// glUniformMatrix4fv(_wfUniformLocations.mvMtx, 1, GL_FALSE, &(mv)[0][0]);
	// glUniformMatrix4fv(_wfUniformLocations.modelMtx, 1, GL_FALSE, &(model)[0][0]);
	// glUniformMatrix3fv(_wfUniformLocations.normalMtx, 1, GL_FALSE, &(normalMtx)[0][0]);
	// glUniformMatrix4fv(_wfUniformLocations.viewportMtx, 1, GL_FALSE, &(viewportMtx)[0][0]);

	_computeAndSendTransformationMatrices(&_wireframeProgram, model, viewMtx, projMtx, _wfUniformLocations.mvpMtx, _wfUniformLocations.modelMtx, _wfUniformLocations.normalMtx);
	_computeAndSendWireframeMatrices(&_wireframeProgram, model, viewMtx, viewportMtx, _wfUniformLocations.viewportMtx, _wfUniformLocations.mvMtx);

	glUniformMatrix4fv(_rtUniformLocations.mvpMtx, 1, GL_FALSE, &(mvp)[0][0]);
	glUniformMatrix3fv(_rtUniformLocations.normalMtx, 1, GL_FALSE, &(normalMtx)[0][0]);

	if(usePhongSpec) {
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_wfSubroutineLocations.phongSpec);
	} else {
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &_wfSubroutineLocations.blinnPhongSpec);
	}

	_setMaterial(Materials::RUBY);

	switch(modelToRender){
		case 1: 
			// glBindVertexArray(icoVAO);
			// glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT,0);
			objects.ico->render();
			break;
		case 2:
			// glBindVertexArray(cubeVAO);
			// glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);
			objects.cube->render();
			break;
		case 3:
			glUseProgram(_teapotProgram);
			model = glm::scale(model, vec3(0.25, 0.25, 0.25));
			model = glm::rotate(model, glm::radians(-90.0f), vec3(1,0,0));
			mv = viewMtx * model;
			nm = glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
			mvp = projMtx * viewMtx * model;

			glUniformMatrix4fv(_tessUniformLocations.mvMtx, 1, GL_FALSE, &(mv)[0][0]);
			glUniformMatrix3fv(_tessUniformLocations.normalMtx, 1, GL_FALSE, &(nm)[0][0]);
			glUniformMatrix4fv(_tessUniformLocations.mvpMtx, 1, GL_FALSE, &(mvp)[0][0]);
			glUniformMatrix4fv(_tessUniformLocations.viewportMtx, 1, GL_FALSE, &(viewportMtx)[0][0]);

			glUniform1i(_tessUniformLocations.tessLevel, tessLevel);
			objects.teapot->render();
			model = glm::mat4(1.0f);
			break;
		case 4:
			objects.sphere->render();
		default: break;
	}

	glUseProgram(_wireframeProgram);
	_setMaterial(Materials::WHITE_PLASTIC);

	model = glm::translate(model, glm::vec3(0.0,1.0,0.0));
	model = glm::rotate(model, glm::radians(90.0f), vec3(1.0,0.0,0.0));
	model = glm::translate(model, glm::vec3(0.0, -1.0, 0.0));
	_computeAndSendTransformationMatrices(&_wireframeProgram, model, viewMtx, projMtx, _wfUniformLocations.mvpMtx, _wfUniformLocations.modelMtx, _wfUniformLocations.normalMtx);
	_computeAndSendWireframeMatrices(&_wireframeProgram, model, viewMtx, viewportMtx, _wfUniformLocations.viewportMtx, _wfUniformLocations.mvMtx);

	glBindVertexArray(groundVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	model = glm::mat4(1.0);
	_computeAndSendTransformationMatrices(&_wireframeProgram, model, viewMtx, projMtx, _wfUniformLocations.mvpMtx, _wfUniformLocations.modelMtx, _wfUniformLocations.normalMtx);
	_computeAndSendWireframeMatrices(&_wireframeProgram, model, viewMtx, viewportMtx, _wfUniformLocations.viewportMtx, _wfUniformLocations.mvMtx);

	glBindVertexArray(groundVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

}

void OGLEngine::_updateScene() {
	float t = float(glfwGetTime());
	_deltaT = t - _tPrev;
	if(_tPrev == 0.0f) _deltaT = 0.0f;
	_tPrev = t;

	vec3 camUp = _arcballCam->getUpVector();
	vec3 camPos = _arcballCam->getPosition();
	vec3 camGaze = vec3(0,0,1);
	
	glUniform3fv(_rtUniformLocations.cameraPos, 1,  &(camPos)[0]);
	glUniform3fv(_rtUniformLocations.cameraUp, 1,  &(camUp)[0]);
	glUniform3fv(_rtUniformLocations.cameraGaze, 1,  &(camGaze)[0]);
	glUniform1i(_wfUniformLocations.wfBoolLocation, showWireFrame);
}

void OGLEngine::run()
{	

	glm::mat4 viewMatrix, projectionMatrix;

    glEnable(GL_DEPTH_TEST);
	glClearColor(0.4,0.4,0.4,0);
	
	glPatchParameteri(GL_PATCH_VERTICES, 16);

    while (!glfwWindowShouldClose(_window))
    {
		glDrawBuffer(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight );

		glViewport( 0, 0, framebufferWidth, framebufferHeight );

		// set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        projectionMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 1000.0f );
		_cameraPos = _arcballCam->getPosition();
		glUniform3fv(_wfUniformLocations.cameraPos, 1, &(_cameraPos)[0]);
		viewMatrix = _arcballCam->getViewMatrix();

		float w2 = framebufferWidth / 2.0f;
        float h2 = framebufferHeight / 2.0f;
        mat4 viewportMatrix = mat4( vec4(w2,0.0f,0.0f,0.0f),
                     vec4(0.0f,h2,0.0f,0.0f),
                     vec4(0.0f,0.0f,1.0f,0.0f),
                     vec4(w2+0, h2+0, 0.0f, 1.0f));
		_updateScene();
		_renderScene(viewMatrix, projectionMatrix, viewportMatrix);
		_showFPS(_window);
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void OGLEngine::shutdown() {
	if(!_isCleanedUp) {
		_cleanupShaders();
		_cleanupBuffers();
		_cleanupTextures();
		_cleanupScene();
		_cleanupOGL();
		_cleanupGLFW();
		_isCleanedUp = true;
	}
}

void OGLEngine::_cleanupShaders() {
	fprintf(stdout, "[INFO]: Cleaning up shaders...\n");
}

void OGLEngine::_cleanupBuffers() {
	fprintf(stdout, "[INFO]: Cleaning up buffers...\n");
	glDeleteVertexArrays(1, &icoVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &teapotVAO);
	glDeleteVertexArrays(1, &groundVAO);
}

void OGLEngine::_cleanupTextures() {
	fprintf(stdout, "[INFO]: Cleaning up textures...\n");
}

void OGLEngine::_cleanupScene() {
	fprintf(stdout, "[INFO]: Cleaning up scene objects...\n");
	delete _arcballCam;
	delete _freeCam;
	delete objects.ico;
	delete objects.cube;
	delete objects.teapot;
	delete objects.sphere;
}

void OGLEngine::_cleanupOGL() {
	// nothing to do here, actually. 
}

void OGLEngine::_cleanupGLFW() {
	fprintf(stdout, "[INFO]: Cleaning up window object...\n");
	glfwDestroyWindow(_window);
	_window = nullptr;
	glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto engine = (OGLEngine*) glfwGetWindowUserPointer(window);

	engine->keyEventHandler(key, action);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	auto engine = (OGLEngine*) glfwGetWindowUserPointer(window);
	engine->mbEventHandler(button, action);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	auto engine = (OGLEngine*) glfwGetWindowUserPointer(window);
	engine->cursorPosEventHandler(glm::vec2(x, y));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	auto engine = (OGLEngine*) glfwGetWindowUserPointer(window);
	engine->scrollWheelEventHandler(yoffset);
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void window_resize_callback(GLFWwindow* window, int width, int height) {
	auto engine = (OGLEngine*) glfwGetWindowUserPointer(window);
	engine->windowResizeHandler(width, height);
}

