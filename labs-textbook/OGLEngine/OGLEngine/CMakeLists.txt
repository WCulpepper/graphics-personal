set(target OGLEngine)

set( ENGINE_SOURCES
	OGLEngine.cpp
	OGLEngine.hpp
	main.cpp
	objects.hpp
	ArcballCam.hpp
	FreeCam.hpp
	Camera.hpp
	 )

add_executable( ${target} ${ENGINE_SOURCES} )

target_compile_definitions(${target}
		PRIVATE
		${DEFAULT_COMPILE_DEFINITIONS}
		GLFW_INCLUDE_NONE
		)

target_include_directories( ${target} PRIVATE ingredients)

target_link_libraries( ${target}
        PRIVATE
        glad
		ingredients
		glfw
        ${OPENGL_gl_LIBRARY}
		)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
