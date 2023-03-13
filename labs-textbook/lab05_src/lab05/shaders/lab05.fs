#version 410

uniform sampler2D RenderTex;
uniform sampler2D DepthTex;

uniform struct LightInfo {
    vec4 Position;
    vec3 Intensity;
} Light;

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
} Material;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 ColorData;
layout (location = 2) out vec3 DepthData;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

vec3 phongModelDiffAndSpec()
{
    vec3 n = Normal;
    vec3 s = normalize(vec3(Light.Position) - Position);
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return diffuse + spec;
}

vec3 blur(float factor, float depth)
{
    float fDistance = 0.7;
    int blurSize;
    blurSize = int(abs(depth - fDistance) * factor);
    vec3 sum = vec3(0, 0, 0);
    vec2 tc = TexCoord;
    int cnt = 0;
    if (abs(depth - 0.7)<0.1) return vec3(texture(RenderTex,TexCoord));
    for (float y = tc.y-blurSize*0.001; y <= tc.y+blurSize*0.001; y+=0.001) {
        for (float x = tc.x-blurSize*0.001; x <= tc.x+blurSize*0.001; x+=0.001) {
            if (x>0 && x<1 && y>0 && y<1) {
                sum += vec3(texture(RenderTex,vec2(x,y)));
                cnt++;
            }
        }
    }
    return sum / cnt;
}


subroutine (RenderPassType)
void SecondPass() {
    FragColor = vec4(blur(20.0, 2.0), 0.5);
}

subroutine (RenderPassType)
void FirstPass() {
    vec3 ambient = Light.Intensity * Material.Ka;
    vec3 diffAndSpec = phongModelDiffAndSpec();
    DepthData = -1.0*(Position.zzz)/30.0;
    ColorData = pow(vec4(diffAndSpec + ambient, 1.0),vec4(1.0 / 2.2)).rgb;
}



void main() {
    RenderPass();
}
