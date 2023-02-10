#version 410 core

uniform float time;
uniform vec3 cameraPos;
uniform vec3 lightPos;

layout(location=0) in vec3 vColor;
layout(location=1) in vec4 vPos;
layout (location=0) out vec4 FragColor;

vec3 diffuse(vec3 s, vec3 n) {
    return vColor*0.05*(dot(n, s));
}

subroutine vec3 processColor(vec3);
subroutine uniform processColor colorProcessor;

subroutine(processColor)
vec3 passThrough(vec3 inputColor) {
    return inputColor;
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
    // vec3 diffColor = diffuse(vPos, vColor);
    FragColor = vec4(vColor, 1.0);
}

