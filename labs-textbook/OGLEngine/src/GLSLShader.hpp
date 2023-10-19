#include <stdexcept>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

class GLSLProgramException : public runtime_error{
public:
    GLSLProgramException (const string &msg) : runtime_error(msg) {}
};

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        TESS_CONTROL = GL_TESS_CONTROL_SHADER,
        TESS_EVAL = GL_TESS_EVALUATION_SHADER,
        COMPUTE = GL_COMPUTE_SHADER
    };
};

class GLSLProgram {
private:
    int handle;
    bool linked;
    map<string, int> uniformLocations;

    GLint getUniformLocation(const char *);
public:
    GLSLProgram();
    ~GLSLProgram();

    GLSLProgram (const GLSLProgram &) = delete;
    GLSLProgram & operator=(const GLSLProgram &) = delete;

    void compileShader(const char* filename);
    void compileShader(const char* filename, GLSLShader::GLSLShaderType type);
    void compileShader(const string &source, GLSLShader::GLSLShaderType type, const char* filename = nullptr);

    void link();
    void use();
    void validate();

    int getHandle();
    bool isLinked();

    void bindAttribLocation(GLuint location, const char* name);
    void bindFragDataLocation(GLuint location, const char* name);
    void setUniform(const char* name, float x, float y, float z);

    void setUniform(const char *name, const glm::vec3 & v);
    void setUniform(const char *name, const glm::vec4 & v);
    void setUniform(const char *name, const glm::mat4 & m);
    void setUniform(const char *name, const glm::mat3 & m);
    void setUniform(const char *name, float val );
    void setUniform(const char *name, int val );
    void setUniform(const char *name, bool val );
    void findUniformLocations();
};