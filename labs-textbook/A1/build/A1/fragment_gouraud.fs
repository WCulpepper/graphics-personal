#version 410 core

uniform float time;

layout(location=0) in vec3 vColor;
layout (location=0) out vec4 FragColor;

// subroutine(processColor)
// vec3 grayScale(vec3 inputColor) {
//     return vec3((inputColor.r + inputColor.g + inputColor.b)/3.0);
// }

// subroutine(processColor)
// vec3 timeFlow(vec3 inputColor) {
//     return inputColor * vec3(   (sin(time)+1.0)/2.0,
//                                 (cos(time)+1.0)/2.0,
//                                 (cos(time)+1.0)/4.0 + (sin(time)+1.0)/4.0 );
// }

void main() {
    FragColor = vec4(vColor, 1.0);
}

