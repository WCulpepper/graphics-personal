#version 410

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;

layout (location=0) out vec3 vColor;

uniform mat4 MVP;

void main()
{
    vColor = VertexColor;

    gl_Position = MVP*vec4(VertexPosition,1.0);
}
