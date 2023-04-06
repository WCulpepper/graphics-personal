#version 460 core

uniform float time;
uniform vec3 cameraPos;
uniform sampler2D tex1;

vec3 lightPos = vec3(3.0,3.0,3.0);
vec3 lightColor = vec3(1.0,1.0,1.0);
float lightIntensity = 1.0;

layout (location=0) out vec4 FragColor;

uniform struct LineInfo {
    float width;
    vec4 color;
} Line;

layout (location = 0) in vec3 gNormal;
layout (location = 1) in vec3 gPosition;
layout(location = 2) in vec2 texCoord;
noperspective in vec3 gEdgeDistance;

vec3 texColor = texture(tex1, texCoord).rgb;

vec3 ambient = vec3(0.05, 0.0, 0.075);
vec3 objectColor = vec3(0.67, 0.0, 1.0);
float materialSI = 0.5;


vec3 diffusePhong(vec3 s, vec3 n) {
    vec3 lightDir = normalize(s - gPosition);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff*lightColor;
    return diffuse;
}

subroutine vec3 processSpecular();
subroutine uniform processSpecular specularProcessor;

subroutine(processSpecular)
vec3 specularPhong() {
    vec3 lightDir = normalize(lightPos - gPosition);
    vec3 view = normalize(cameraPos - gPosition);
    vec3 r = reflect(-lightDir, gNormal);
    float spec = pow(max(dot(view, r), 0.0), 32);
    vec3 specular = materialSI*spec*lightColor*100;
    return specular;
}

subroutine(processSpecular)
vec3 specularBlinnPhong() {
    vec3 lightDir = normalize(lightPos - gPosition);
    vec3 view = normalize(cameraPos - gPosition);
    vec3 halfway = normalize(view + lightDir);
    float spec = pow(max(dot(halfway, gNormal), 0.0), 32);
    vec3 specular = materialSI*spec*lightColor*100;
    return specular;
}

void main() {
    
    float d = min(gEdgeDistance.x, gEdgeDistance.y);
    d = min(d, gEdgeDistance.z);

    float mixVal = smoothstep(Line.width - 1, Line.width + 1, d);

    vec3 color = (diffusePhong(lightPos, gNormal) + specularProcessor() + ambient) * objectColor;
    // FragColor = mix(Line.color, vec4(color, 1.0), mixVal);
    //FragColor = vec4(gPosition, 1.0);
    // FragColor = vec4(gNormal, 1.0);
    FragColor = vec4(texCoord, 1.0, 1.0);
}

