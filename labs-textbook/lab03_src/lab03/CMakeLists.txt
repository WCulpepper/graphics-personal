set(target lab03)
set( lab03_SOURCES
	lab03.cpp
	)

add_executable( ${target} ${lab03_SOURCES} )

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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/surfaceData.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
