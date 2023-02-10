#version 410 core

uniform float time;

uniform vec3 cameraPos;
uniform vec3 lightPos;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;
layout (location=2) in vec3 vertexNormal;

layout (location=0) out vec3 vColor;

uniform mat4 MVP;

vec3 diffuse(vec3 s, vec3 n) {
    return VertexColor*1.0*1.0*(dot(n, s));
}

subroutine vec3 processVertex(vec3);
subroutine uniform processVertex vertexProcessor;

subroutine(processVertex)
vec3 passThrough(vec3 inputVertex) {
    return inputVertex;
}

// subroutine(processVertex)
// vec3 timeFlow(vec3 inputVertex) {
//     return inputVertex + vec3(  cos(inputVertex.x + time),
//                                 sin(inputVertex.z + time),
//                                 0
//                              );
// }

// subroutine(processVertex)
// vec3 shrinkObj(vec3 inputVertex) {
//     return vec3(inputVertex.x/2.0, inputVertex.y/2.0, inputVertex.z/2.0);
// }

// subroutine(processVertex)
// vec3 gouraudBlinnPhong(vec3 inputVertex) {
//     return inputVertex;
// }

// subroutine(processVertex)
// vec3 gouraudPhong(vec3 inputVertex) {
//     return inputVertex;
// }



void main()
{
    // vec3 vNormal = normalize(VertexPosition);
    // vColor = diffuse(VertexColor, vNormal);

    vColor = VertexColor;

    gl_Position = MVP * vec4(VertexPosition,1.0);
}