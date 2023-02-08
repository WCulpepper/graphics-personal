#version 410

in vec3 texCoord;
out vec4 fragColor;

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

    return (n + 1.) / 2.;
}

void main(void)
{

    float a = 0;

    init();
    
    a = perlin(texCoord,5);
    fragColor = vec4(a, a, a, 1);
}
