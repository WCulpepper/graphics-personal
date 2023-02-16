#version 410 core

uniform float time;
uniform vec3 cameraPos;

uniform mat3 normalMtx;
vec3 lightPos = vec3(3.0,3.0,3.0);
vec3 lightColor = vec3(1.0,1.0,1.0);

float lightIntensity = 1.0;

vec3 ambient = vec3(0.05, 0.0, 0.075);
vec3 objectColor = vec3(0.67, 0.0, 1.0);
vec3 materialSpec = vec3(1.0, 1.0, 0.0);
float materialSI = 0.5;

layout(location=0) in vec3 vColor;
layout(location=1) in vec3 vPos;
layout(location=2) in vec3 vNorm;
layout (location=0) out vec4 FragColor;

vec3 diffusePhong(vec3 s, vec3 n) {
    vec3 lightDir = normalize(s - vPos);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff*lightColor;
    return diffuse;
}

subroutine vec3 processSpecular();
subroutine uniform processSpecular specularProcessor;

subroutine(processSpecular)
vec3 specularPhong() {
    vec3 lightDir = normalize(lightPos - vPos);
    vec3 view = normalize(cameraPos - vPos);
    vec3 r = reflect(-lightDir, vNorm);
    float spec = pow(max(dot(view, r), 0.0), 32);
    vec3 specular = materialSI*spec*lightColor*100;
    return specular;
}

subroutine(processSpecular)
vec3 specularBlinnPhong() {
    vec3 lightDir = normalize(lightPos - vPos);
    vec3 view = normalize(cameraPos - vPos);
    vec3 halfway = normalize(view + lightDir);
    float spec = pow(max(dot(halfway, vNorm), 0.0), 32);
    vec3 specular = materialSI*spec*lightColor*100;
    return specular;
}

// subroutine(processColor)
// vec3 timeFlow(vec3 inputColor) {
//     return inputColor * vec3(   (sin(time)+1.0)/2.0,
//                                 (cos(time)+1.0)/2.0,
//                                 (cos(time)+1.0)/4.0 + (sin(time)+1.0)/4.0 );
// }

// subroutine(processColor)
// vec3 grayScale(vec3 inputColor) {
//     return vec3((inputColor.r + inputColor.g + inputColor.b)/3.0);
// }

void main() {
    vec3 color = (diffusePhong(lightPos, vNorm) + specularProcessor() + ambient) * objectColor;
    FragColor = vec4(color, 1.0);
}

