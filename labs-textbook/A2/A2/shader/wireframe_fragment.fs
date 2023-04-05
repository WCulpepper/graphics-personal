#version 460 core

uniform float time;
uniform vec3 cameraPos;

uniform LightProperties {
    vec3 lightPos;
    vec3 lightColor;
    float lightIntensity;
};

uniform MaterialProperties {
    vec3 ambient;
    vec3 objectColor;
    float materialSI;
};


layout (location=0) out vec4 FragColor;

uniform struct LineInfo {
    float width;
    vec4 color;
} Line;

in vec3 gPosition;
in vec3 gNormal;
noperspective in vec3 gEdgeDistance;


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
    float d = min(gEdgeDistance.x, gEdgeDistance.y);
    d = min(d, gEdgeDistance.z);

    float mixVal = smoothstep(Line.width - 1, Line.width + 1, d);

    vec3 color = (diffusePhong(lightPos, gNormal) + specularProcessor() + ambient) * objectColor;
    FragColor = mix(Line.color, vec4(color, 1.0), mixVal);
    // FragColor = vec4(gPosition, 1.0);
    // FragColor = vec4(gNormal, 1.0);
}

