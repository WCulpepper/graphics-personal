#include "OGLEngine.hpp"

int main() {
    auto engine = new OGLEngine(4, 6, 1280, 720, "Final Project: Raytracing with Triangle Meshes");
    engine-> initialize();

    if(engine->getError() == OGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        engine->run();
    }
    engine->shutdown();
    delete engine;
    return EXIT_SUCCESS;
}