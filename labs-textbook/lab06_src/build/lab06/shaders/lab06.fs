#version 410

uniform samplerCube CubeMapTex;
uniform float ReflectFactor;

in vec3 LightIntensity;
in vec3 ReflectDir;

layout( location = 0 ) out vec4 FragColor;

void main() {
    // Access the cube map texture
    vec4 cubeMapColor = texture(CubeMapTex, ReflectDir);
    FragColor = mix( vec4(LightIntensity,1.0), cubeMapColor, ReflectFactor);
}
