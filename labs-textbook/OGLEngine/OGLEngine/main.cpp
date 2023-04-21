#include "OGLEngine.hpp"

int main() {
    std::cout << "In main\n";
    // auto engine = new OGLEngine(4, 6, 1280, 720, "OpenGL Engine");
    OGLEngine* engine = new OGLEngine();
    if(engine) std::cout << "Created engine object\n"; 
    engine-> initialize();

    if(engine->getError() == OGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        engine->run();
    }
    engine->shutdown();
    delete engine;
    return EXIT_SUCCESS;
}