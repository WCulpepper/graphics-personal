#version 410

layout(location=0) in vec2 inVertex;
layout(location=1) in vec2 inTexture;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;

out vec2 texCoord;

void main(void)
{
    texCoord = inTexture;

    gl_Position = projectionMatrix * viewingMatrix * vec4(inVertex, 0, 1);
}

