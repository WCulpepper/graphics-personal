#version 410

layout(location=0) in vec2 inVertex;
layout(location=1) in vec2 inTexture;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelMatrix;

out vec3 texCoord;

void main(void)
{
    texCoord = texCoord = vec3(modelMatrix * vec4(inTexture, 0, 1));

    gl_Position = projectionMatrix * viewingMatrix * vec4(inVertex, 0, 1);
}

