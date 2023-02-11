#version 410 core

uniform float time;
uniform vec3 cameraPos;
uniform vec3 lightPos;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;
layout (location=2) in vec3 VertexNormal;

layout (location=0) out vec3 vColor;
layout (location=1) out vec4 vPos;
layout (location=2) out vec3 vNorm;

uniform mat4 MVP;

void main()
{
    vColor = VertexColor;
    vNorm = normalize(VertexNormal);

    gl_Position = MVP * vec4(VertexPosition, 1.0);

    vPos = MVP * vec4(VertexPosition, 1.0);
}