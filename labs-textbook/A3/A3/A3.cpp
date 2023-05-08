#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLuint gVAO, gPrg;
GLint gViewingMatrixLoc, gModelingMatrixLoc, gProjectionMatrixLoc;
int gIndexCount;
int gWidth, gHeight;
bool gLeftPressed = false, gResChange = false;

glm::vec3 gGaze(0, 0, -1);
glm::vec3 gUp(0, 1, 0);
glm::vec3 gEyePos(0, 0, 3);

// ray tracing parameters
glm::vec3 camPos(0,2,0);
glm::vec3 camGaze(0,0,-1);
glm::vec3 camUp(0,1,0);
glm::vec3 sphere1Pos(1,1,-8);
float sphere1Radius=1.0f;
glm::vec3 sphere1Mat(1.0,0.4,0.5);
float sphere1Reflect=0.3f;
glm::vec3 sphere2Pos(0,0.8,-7);
float sphere2Radius=0.8f;
glm::vec3 sphere2Mat(0.3,0.3,1.0);
float sphere2Reflect=0.0f;

bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

GLuint createVS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

	return vs;
}

GLuint createFS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

	return fs;
}

void initShaders()
{
    const char* vsName = "shaders/A3.vs";
    const char* fsName = "shaders/A3.fs";

    gPrg = glCreateProgram();

    GLuint vs = createVS(vsName);
    GLuint fs = createFS(fsName);

    glAttachShader(gPrg, vs);
    glAttachShader(gPrg, fs);

    glLinkProgram(gPrg);
    GLint status;
    glGetProgramiv(gPrg, GL_LINK_STATUS, &status);

    if (status != GL_TRUE)
    {
        cout << "Program link failed: " << endl;
        exit(-1);
    }

	gViewingMatrixLoc = glGetUniformLocation(gPrg, "viewingMatrix");
	gProjectionMatrixLoc = glGetUniformLocation(gPrg, "projectionMatrix");

    glUseProgram(gPrg);
}

GLuint initVBO()
{
    GLuint vao, vertexAttribBuffer, indexBuffer;

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

    return vao;
}

void init() 
{
    initShaders();
    gVAO = initVBO();
}

void drawModels()
{
    glUseProgram(gPrg);
    glBindVertexArray(gVAO);

    float fovyRad = (float) (45.0 / 180.0) * M_PI;

    glm::mat4 viewingMatrix = glm::lookAt(gEyePos, gEyePos + gGaze, gUp);
    glm::mat4 projectionMatrix = glm::perspective(fovyRad, 1.0f, 0.1f, 30.0f);

	glUniformMatrix4fv(gProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(gViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));

    glUniform3fv(glGetUniformLocation(gPrg, "sphere1Pos"),1,glm::value_ptr(sphere1Pos));
    glUniform3fv(glGetUniformLocation(gPrg, "sphere1Mat"),1,glm::value_ptr(sphere1Mat));
    glUniform1f(glGetUniformLocation(gPrg, "sphere1Radius"),sphere1Radius);
    glUniform1f(glGetUniformLocation(gPrg, "sphere1Reflect"),sphere1Reflect);
    glUniform3fv(glGetUniformLocation(gPrg, "sphere2Pos"),1,glm::value_ptr(sphere2Pos));
    glUniform3fv(glGetUniformLocation(gPrg, "sphere2Mat"),1,glm::value_ptr(sphere2Mat));
    glUniform1f(glGetUniformLocation(gPrg, "sphere2Radius"),sphere2Radius);
    glUniform1f(glGetUniformLocation(gPrg, "sphere2Reflect"),sphere2Reflect);
    glUniform3fv(glGetUniformLocation(gPrg, "camPos"),1,glm::value_ptr(camPos));
    glUniform3fv(glGetUniformLocation(gPrg, "camGaze"),1,glm::value_ptr(camGaze));
    glUniform3fv(glGetUniformLocation(gPrg, "camUp"),1,glm::value_ptr(camUp));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void display()
{

    // set background to black
    glClearColor(0.0, 0.0, 0.0, 0);

    glClear(GL_COLOR_BUFFER_BIT);

    drawModels();

}

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    if (w != gWidth || h != gHeight)
    {
        gResChange = true;
    }

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_W)
        sphere2Pos.y+=0.1;
    else if (key == GLFW_KEY_S)
        sphere2Pos.y-=0.1;
    else if (key == GLFW_KEY_A)
        sphere2Pos.x-=0.1;
    else if (key == GLFW_KEY_D)
        sphere2Pos.x+=0.1;
    else if (key == GLFW_KEY_Z)
        sphere2Pos.z+=0.1;
    else if (key == GLFW_KEY_X)
        sphere2Pos.z-=0.1;
    else if (key == GLFW_KEY_UP)
        camPos.y+=0.1;
    else if (key == GLFW_KEY_DOWN)
        camPos.y-=0.1;
    else if (key == GLFW_KEY_LEFT)
        camPos.x-=0.1;
    else if (key == GLFW_KEY_RIGHT)
        camPos.x+=0.1;
}

void rotateCamera(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse = true;
    static double lastX, lastY;
    static float tPitch = 0.5;
    static float yaw = 0;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (!gLeftPressed)
    {
        return;
    }


    float sensitivity = 0.3;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    tPitch += yoffset / 100;
    yaw += xoffset;

    tPitch = tPitch > 1 ? 1 : tPitch < 0 ? 0 : tPitch;

    glm::vec3 upAxis = glm::vec3(0, 1, -0.01);
    glm::vec3 downAxis = glm::vec3(0, -1, -0.01);

    float sint = sin(glm::radians(yaw/2));
    glm::quat yawQuat(cos(glm::radians(yaw/2)), sint * 0, sint * 1, sint * 0);
    upAxis = glm::toMat3(yawQuat) * upAxis;
    downAxis = glm::toMat3(yawQuat) * downAxis;

    glm::quat upQuat = glm::quat(0, upAxis);
    glm::quat downQuat = glm::quat(0, downAxis);

    gGaze = glm::axis(glm::mix(downQuat, upQuat, tPitch));
    //Up = glm::axis(glm::mix(glm::quat(0, 0, 1, 0), glm::quat(0, 0, 0, -1), tPitch));

    //cout << "tPitch = " << tPitch << endl;
    //cout << "yaw = " << yaw << endl;
    //cout << "gaze = " << gGaze.x << "\t" << gGaze.y << "\t" << gGaze.z << endl;
    //cout << "up   = " << gUp.x << "\t" << gUp.y << "\t" << gUp.z << endl;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        gLeftPressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        gLeftPressed = false;
    }
}

void mainLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    gWidth = 720;
    gHeight = 720;
    window = glfwCreateWindow(gWidth, gHeight, "Lab 7: Ray Tracing", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // ensure vsync

    gladLoadGL();

    init();

    glfwSetKeyCallback(window, keyboard);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, rotateCamera);
    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, gWidth, gHeight); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
