set(target ingredients)

set(ingredients_SOURCES
        glad/src/glad.c
	glslprogram.cpp
	glslprogram.h
	scene.h
	scenerunner.h
	teapot.cpp
	teapot.h
	teapotdata.h
	teapotpatch.cpp
	teapotpatch.h
	cookbookogl.h
	glutils.h
	glutils.cpp
	trianglemesh.h
	trianglemesh.cpp
	drawable.h)

add_library(${target} STATIC ${ingredients_SOURCES})

target_include_directories(${target} PUBLIC glad/include)

if( UNIX AND NOT APPLE )
    target_link_libraries(${target} PUBLIC ${CMAKE_DL_LIBS})
endif()
