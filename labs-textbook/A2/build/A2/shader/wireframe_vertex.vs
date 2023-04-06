#version 460 core

uniform mat4 modelMtx;
uniform mat3 normalMtx;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;

layout (location=0) out vec3 vPos;
layout (location=1) out vec3 vNorm;

uniform mat4 MVP;
uniform mat4 MV;

void main()
{
    vNorm = normalize(VertexNormal);
    vPos = vec3(MV * vec4(VertexPosition, 1.0));
    gl_Position = MVP * vec4(VertexPosition, 1.0);    
}