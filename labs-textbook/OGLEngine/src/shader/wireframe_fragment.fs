#version 460 core

uniform float time;
uniform vec3 cameraPos;
uniform sampler2D tex1;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity = 1.0;

uniform vec4 DarkWoodColor = vec4( 0.8, 0.5, 0.1, 1.0 );
uniform vec4 LightWoodColor = vec4( 1.0, 0.75, 0.25, 1.0 );
uniform mat4 Slice = mat4(1.0);

uniform MaterialSettings {
    float materialDiffuse[4];
    float materialSpecular[4];
    float materialShininess;
    float materialAmbient[4];
};

uniform bool standardShading = true;
uniform bool showWireFrame;

vec3 corners[12] = {
    vec3(1,1,0),vec3(-1,1,0),vec3(1,-1,0),vec3(-1,-1,0),
    vec3(1,0,1),vec3(-1,0,1),vec3(1,0,-1),vec3(-1,0,-1),
    vec3(0,1,1),vec3(0,-1,1),vec3(0,1,-1),vec3(0,-1,-1)
};

layout (location=0) out vec4 FragColor;
layout (location=1) out vec3 PositionData;
layout (location=2) out vec3 NormalData;
layout (location=3) out vec3 ColorData;

layout (binding=0) uniform sampler2D PositionTex;
layout (binding=1) uniform sampler2D NormalTex;
layout (binding=2) uniform sampler2D ColorTex;

uniform struct LineInfo {
    float width;
    vec4 color;
} Line;

layout (location = 0) in vec3 gNormal;
layout (location = 1) in vec3 gPosition;
// layout(location = 2) in vec2 tCoord;
noperspective in vec3 gEdgeDistance;

// vec3 texColor = texture(tex1, tCoord).rgb;

vec3 diffusePhong(vec3 s, vec3 n) {
    vec3 lightDir = normalize(s - gPosition);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff*lightColor;
    if(standardShading)
        return diffuse*vec3(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2]);
    else
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
    vec3 specular = materialShininess*spec*lightColor*100;
    if(standardShading)
        return specular*vec3(materialSpecular[0], materialSpecular[1], materialSpecular[2]);
    else
        return specular;
}

subroutine(processSpecular)
vec3 specularBlinnPhong() {
    vec3 lightDir = normalize(lightPos - gPosition);
    vec3 view = normalize(cameraPos - gPosition);
    vec3 halfway = normalize(view + lightDir);
    float spec = pow(max(dot(halfway, gNormal), 0.0), 32);
    vec3 specular = materialShininess*spec*lightColor*100;
    if(standardShading)
        return specular*vec3(materialSpecular[0], materialSpecular[1], materialSpecular[2]);
    else
        return specular;
}

subroutine void RenderPass();
subroutine uniform RenderPass pass;

subroutine (RenderPass)
void pass1() {

}

subroutine (RenderPass)
void pass2() {

}

vec3 gradients[16];
int table[16];

void init() {
    int i;
    gradients[0] = vec3(0, -1, -1);
    gradients[1] = vec3(1, 0, -1);
    gradients[2] = vec3(0, -1, 1);
    gradients[3] = vec3(0, 1, -1);
    gradients[4] = vec3(1, -1, 0);
    gradients[5] = vec3(1, 1, 0);
    gradients[6] = vec3(-1, 1, 0);
    gradients[7] = vec3(0, 1, 1);
    gradients[8] = vec3(-1, 0, -1);
    gradients[9] = vec3(1, 1, 0);
    gradients[10] = vec3(-1, 1, 0);
    gradients[11] = vec3(-1, -1, 0);
    gradients[12] = vec3(1, 0, 1);
    gradients[13] = vec3(-1, 0, 1);
    gradients[14] = vec3(0, -1, 1);
    gradients[15] = vec3(0, -1, -1);
    for (i=0;i<16;i++)
        table[i]=i;
}


float smoothingFunc(float t)
{
    t = (t > 0.) ? t : -t;

    float t3 = t * t * t;
    float t4 = t3 * t;

    return -6 * t4 * t + 15 * t4 - 10 * t3 + 1.;
}


float randomNumber(float u, float v, float w, int i, int j, int k)
{
    int idx;
    idx = table[abs(k) % 16];
    idx = table[abs(j + idx) % 16];
    idx = table[abs(i + idx) % 16];

    vec3 gijk = gradients[idx];
    vec3 uvw = vec3(u, v, w);

    return smoothingFunc(u) * smoothingFunc(v) * smoothingFunc(w) * dot(gijk, uvw); 
}

float perlin(vec3 pos, float scalingFactor)
{
    float x = scalingFactor * pos.x;
    float y = scalingFactor * pos.y;
    float z = scalingFactor * pos.z;

    int xmin = int(floor(x));
    int ymin = int(floor(y));
    int zmin = int(floor(z));

    float n = 0;
    for (int i = xmin; i <= xmin + 1; ++i)
    {
        for (int j = ymin; j <= ymin + 1; ++j)
        {
            for (int k = zmin; k <= zmin + 1; ++k)
            {
                n += randomNumber(x - i, y - j, z - k, i, j, k);
            }
        }
    }

    // return (n + 1.) / 2.;
    return abs(n);
}

void main() {
    init();

    vec4 cyl = Slice*vec4(gPosition.xy, 1.0, 1.0);
    float dist = length(cyl.xz);
    vec4 noise =  vec4(1.0)*perlin(gPosition, 1.0);
    dist += noise.b;

    float t = 1.0 - abs( fract( dist ) * 2.0 - 1.0 );
    t = smoothstep( 0.2, 0.5, t );
    vec4 woodColor = mix( DarkWoodColor, LightWoodColor, t );

    float d = min(gEdgeDistance.x, gEdgeDistance.y);
    d = min(d, gEdgeDistance.z);

    float mixVal = smoothstep(Line.width - 1, Line.width + 1, d);

    vec3 color = (diffusePhong(lightPos, gNormal) + specularProcessor() + vec3(materialAmbient[0], materialAmbient[1], materialAmbient[2]));
    if(standardShading)
        if(showWireFrame)
            FragColor = mix(Line.color, vec4(color, 1.0), mixVal);
        else   
            FragColor = vec4(color, 1.0);
    else
        FragColor = mix(Line.color, vec4(color, 1.0)*woodColor, mixVal);
    // FragColor = vec4(1.0,1.0,1.0,1.0);
    // FragColor = vec4(gPosition, 1.0);
    // FragColor = vec4(gNormal, 1.0);
    // FragColor = vec4(tCoord, 1.0, 1.0);
}

