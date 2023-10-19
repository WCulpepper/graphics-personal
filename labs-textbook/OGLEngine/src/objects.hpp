#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Objects {

    struct Vertex {
        GLfloat x, y, z, normX, normY, normZ, texU, texV;
    };

    struct Texture {
        GLuint id;
        const char* type;
    };

    class Object {
    public:
        Object(GLfloat size = 1.0f) {
            initBuffers();
        }

        virtual void initBuffers() = 0;
        virtual void render() = 0;
    };

    class Cube : public Object {
    public:
        Cube(GLfloat size=1.0f) {
            initBuffers();
        }

        void initBuffers() {
            GLuint cube_vertpos_buffer;
            GLuint cube_normal_buffer;
            GLuint cube_index_buffer;
        }

    };
}