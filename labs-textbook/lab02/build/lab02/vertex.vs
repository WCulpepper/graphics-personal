#version 410 core

uniform float time;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;

layout (location=0) out vec3 vColor;

uniform mat4 MVP;

subroutine vec3 processVertex(vec3);
subroutine uniform processVertex vertexProcessor;

subroutine(processVertex)
vec3 passThrough(vec3 inputVertex) {
    return inputVertex;
}

subroutine(processVertex)
vec3 timeFlow(vec3 inputVertex) {
    return inputVertex + vec3(  cos(inputVertex.x + time),
                                sin(inputVertex.z + time),
                                0
                             );
}

void main()
{
    vColor = VertexColor;

    gl_Position = MVP * vec4(vertexProcessor(VertexPosition),1.0);
}