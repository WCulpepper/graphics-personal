set(target glad)

set(glad_SOURCES
        glad/src/glad.c)

add_library(${target} STATIC ${glad_SOURCES})

target_include_directories(${target} PUBLIC glad/include)

if( UNIX AND NOT APPLE )
    target_link_libraries(${target} PUBLIC ${CMAKE_DL_LIBS})
endif()