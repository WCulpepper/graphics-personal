#version 410

layout(location=0) in vec2 inVertex;
layout(location=1) in vec2 inTexture;

uniform mat4 projectionMtx;
uniform mat4 viewMtx;


out vec2 tCoord;

void main(void)
{
    tCoord = inTexture;

    gl_Position = projectionMtx * viewMtx * vec4(inVertex, 0, 1);
}