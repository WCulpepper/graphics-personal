#version 410 core

uniform float time;

uniform vec3 cameraPos;
uniform vec3 lightPos;

float lightIntensity = 10.0;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;
layout (location=2) in vec3 VertexNormal;

layout (location=0) out vec3 vColor;

uniform mat4 MVP;

vec3 diffusePhong(vec3 s, vec3 n) {
    return material.diffuse*lightIntensity*(dot(n, s));
}

subroutine vec3 processVertex(vec3);
subroutine uniform processVertex vertexProcessor;

subroutine(processVertex)
vec3 passThrough(vec3 inputVertex) {
    return inputVertex;
}



void main()
{
    vec3 vNorm = normalize(VertexPosition);
    // vNorm = vec3(0, 1, 0);
    vColor = diffusePhong(vNorm, lightPos);

    // vColor = VertexColor;

    gl_Position = MVP * vec4(VertexPosition,1.0);
}