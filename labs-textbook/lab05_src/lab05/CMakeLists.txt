set(target lab05)
set( lab05_SOURCES
	lab05.cpp
	lab05.h
	)

add_executable( ${target} ${lab05_SOURCES} )

target_compile_definitions(${target}
		PRIVATE
		GLFW_INCLUDE_NONE
		)

target_link_libraries( ${target}
		PRIVATE
		ingredients
		glfw
		${OPENGL_gl_LIBRARY}
		)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
