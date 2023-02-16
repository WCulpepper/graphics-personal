#version 410 core

uniform float time;

uniform vec3 cameraPos;
vec3 lightPos = vec3(3.0,3.0,3.0);
vec3 lightColor = vec3(1.0,1.0,1.0);

float lightIntensity = 0.05;

vec3 ambient = vec3(0.05, 0.0, 0.075);
vec3 objectColor = vec3(0.67, 0.0, 1.0);
vec3 materialSpec = vec3(1.0, 1.0, 0.0);
float materialSI = 0.5;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;
layout (location=2) in vec3 VertexNormal;

layout (location=0) out vec3 vColor;

uniform mat4 MVP;
uniform mat4 modelMtx;
uniform mat3 normalMtx;

vec3 diffusePhong(vec3 s, vec3 n) {
    vec3 vPos = vec3(modelMtx * vec4(VertexPosition, 1.0));
    vec3 lightDir = normalize(s - vPos);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff*lightColor;
    return diffuse;
}

subroutine vec3 processSpecular();
subroutine uniform processSpecular specularProcessor;

subroutine(processSpecular)
vec3 specularPhong() {
    vec3 lightDir = normalize(lightPos - VertexPosition);
    vec3 view = normalize(cameraPos - VertexPosition);
    vec3 r = reflect(-lightDir, VertexNormal);
    float spec = pow(max(dot(view, r), 0.0), 32);
    vec3 specular = materialSI*spec*lightColor*100;
    return specular;
}

subroutine(processSpecular)
vec3 specularBlinnPhong() {
    vec3 lightDir = normalize(lightPos - VertexPosition);
    vec3 view = normalize(cameraPos - VertexPosition);
    vec3 halfway = normalize(view + lightDir);
    float spec = pow(max(dot(halfway, VertexNormal), 0.0), 32);
    vec3 specular = materialSI*spec*lightColor*100;
    return specular;
}



void main()
{
    vec3 vNorm = normalize(VertexNormal);
    // vNorm = vec3(0, 1, 0);
    vColor = (diffusePhong(lightPos, vNorm) + specularProcessor() + ambient) * objectColor;
    // vColor = VertexColor;

    gl_Position = MVP * vec4(VertexPosition,1.0);
}