#version 460 core

uniform float time;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform mat4 modelMtx;
uniform mat3 normalMtx;

layout (location=0) in vec3 VertexPosition;
layout (location=2) in vec3 VertexNormal;

layout (location=1) out vec3 vPos;
layout (location=2) out vec3 vNorm;

uniform mat4 MVP;

void main()
{
    vNorm = normalize(normalMtx*VertexNormal);

    gl_Position = MVP * vec4(VertexPosition, 1.0);

    vPos = vec3(modelMtx * vec4(VertexPosition, 1.0));
}