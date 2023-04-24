#version 400

uniform float LineWidth;
uniform vec4 LineColor;
uniform vec4 LightPosition;
uniform vec3 LightIntensity;
uniform vec3 Kd;

uniform vec4 DarkWoodColor = vec4( 0.8, 0.5, 0.1, 1.0 );
uniform vec4 LightWoodColor = vec4( 1.0, 0.75, 0.25, 1.0 );
uniform mat4 Slice = mat4(1.0);

noperspective in vec3 EdgeDistance;
in vec3 Normal;
in vec4 Position;

layout ( location = 0 ) out vec4 FragColor;

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

vec3 diffuseModel( vec3 pos, vec3 norm )
{
    vec3 s = normalize(vec3(LightPosition) - pos);
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = LightIntensity * Kd * sDotN;

    return diffuse;
}

float edgeMix()
{
    // Find the smallest distance
    float d = min( min( EdgeDistance.x, EdgeDistance.y ), EdgeDistance.z );

    if( d < LineWidth - 1 ) {
        return 1.0;
    } else if( d > LineWidth + 1 ) {
        return 0.0;
    } else {
        float x = d - (LineWidth - 1);
        return exp2(-2.0 * (x*x));
    }
}

void main()
{
    init();

    vec4 cyl = Slice*vec4(Position.xy, 1.0, 1.0);
    float dist = length(cyl.xz);
    vec4 noise =  vec4(1.0)*perlin(Position.xyz, 1.0);
    dist += noise.b;

    float t = 1.0 - abs( fract( dist ) * 2.0 - 1.0 );
    t = smoothstep( 0.2, 0.5, t );
    vec4 woodColor = mix( DarkWoodColor, LightWoodColor, t );

    float d = min(EdgeDistance.x, EdgeDistance.y);
    d = min(d, EdgeDistance.z);

    float mixVal = smoothstep(LineWidth - 1, LineWidth + 1, d);
    vec4 color = vec4( diffuseModel( Position.xyz, Normal ), 1.0);
    color = pow( color, vec4(1.0/2.2) );
    FragColor = mix( color*woodColor, LineColor, mixVal );
    // FragColor = color;
    // FragColor = vec4(1.0,1.0,1.0, 1.0);
}
