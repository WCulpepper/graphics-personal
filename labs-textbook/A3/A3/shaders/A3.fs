#version 410

uniform vec3 camPos;
uniform vec3 camGaze;
uniform vec3 camUp;
uniform vec3 sphere1Pos;
uniform float sphere1Radius;
uniform vec3 sphere1Mat;
uniform float sphere1Reflect;
uniform vec3 sphere2Pos;
uniform float sphere2Radius;
uniform vec3 sphere2Mat;
uniform float sphere2Reflect;

in vec2 texCoord;
out vec4 fragColor;

struct PlasticMaterial {
    vec3 ambient = vec3(0.1,0.1,0.1);
    vec3 diffuse = vec3(0.55,0.55,0.55);
    vec3 specular = vec3(0.7,0.7,0.7);
    vec3 BRDF = diffuse/radians(180);
    float shininess = 0.25;
} whitePlastic;

int maxDepth = 2;

vec3 rayo, rayd;
void generateRay() {
    int d; // distance of the image plane from the camera
    vec3 m,q; // m is the middle of the image, q is the bottom left point of the image
    vec3 camRight;
    vec3 pixelCenter;
    d = 1; // distance of camera to image plane
    m = camPos+camGaze*d;
    camRight = cross(camGaze,camUp); // the direction of the camera's +x axis
    q = m+(-0.5*camRight)+(-0.5*camUp); // we assume image plane left boundary is at x=-0.5 and right boundary at x=0.5
    pixelCenter = q + (texCoord.x)*camRight + (texCoord.y)*camUp;
    rayo = camPos; // ray origin
    rayd = pixelCenter-camPos; // ray direction
}

float intersectSphere(vec3 rayo, vec3 rayd, vec3 center, float r)
{
    float A,B,C; //constants for the quadratic function
    float delta;
    float t,t1,t2;
    C = (rayo.x-center.x)*(rayo.x-center.x)+(rayo.y-center.y)*(rayo.y-center.y)+(rayo.z-center.z)*(rayo.z-center.z)-r*r;
    B = 2*rayd.x*(rayo.x-center.x)+2*rayd.y*(rayo.y-center.y)+2*rayd.z*(rayo.z-center.z);
    A = rayd.x*rayd.x+rayd.y*rayd.y+rayd.z*rayd.z;
    delta = B*B-4*A*C;
    if (delta<0) return -1;
    else if (delta==0) {
        t = -B / (2*A);
    }
    else {
        t1 = (-B + sqrt(delta)) / (2*A);
        t2 = (-B - sqrt(delta)) / (2*A);
        if (t1<t2) t=t1; else t=t2;
    }
    return t;
}

float intersectXZPlane(vec3 rayo, vec3 rayd) {
    float t;
    if (rayd.y==0.0) return -1; // parallel to the xy plane, no intersection
    t = -rayo.y / rayd.y;
    return t;
}

float intersectUpperXZPlane(vec3 rayo, vec3 rayd) {
    float t;
    if (rayd.y==0.0) return -1; // parallel to the xy plane, no intersection
    t = -(rayo.y+5) / (rayd.y+5);
    return t;
}

float intersectLeftYZPlane(vec3 rayo, vec3 rayd) {
    float t;
    if (rayd.x==0.0) return -1; // parallel to the xy plane, no intersection
    t = -(rayo.x-5) / (rayd.x-5);
    return t;
}

float intersectRightYZPlane(vec3 rayo, vec3 rayd) {
    float t;
    if (rayd.x==0.0) return -1; // parallel to the xy plane, no intersection
    t = -(rayo.x+5) / (rayd.x+5);
    return t;
}

float intersectXYPlane(vec3 rayo, vec3 rayd) {
    float t;
    if(rayd.z==0.0) return -1;
    t = -(rayo.z-5)/(rayd.z-5);
}



vec3 computeColorReflect(vec3 p, vec3 n, vec3 mat) {
    vec3 lightPos = vec3(4,16,2);
    vec3 ambient = mat*0.1;
    vec3 lightVec;
    vec3 halfVec;
    vec3 viewVec;
    int specExp;

    float t1;
    float t2;
    float t3;
    float t4;
    float t5;
    float t6;
    float t7;
    int shadow;

    // check for shadow
    t1 = intersectSphere(p,lightPos-p,sphere1Pos,sphere1Radius);
    t2 = intersectSphere(p,lightPos-p,sphere2Pos,sphere2Radius);
    t3 = intersectXZPlane(p,lightPos-p);
    t4 = intersectUpperXZPlane(p,lightPos-p);
    t5 = intersectXYPlane(p,lightPos-p);
    t6 = intersectLeftYZPlane(p,lightPos-p);
    t7 = intersectRightYZPlane(p,lightPos-p);
    if ((t1>0.0001 && t1<1) || (t2>0.0001 && t2<1) || (t3>0.0001 && t3<1) || (t4>0.0001 && t4<1) || (t5>0.0001 && t5<1) || (t6>0.0001 && t6<1) || (t7>0.0001 && t7<1))
        shadow = 1;
    else
        shadow = 0;

    if(shadow == 1)
        return ambient;

    // compute diffuse and specular
    specExp = 100;
    lightVec = normalize(lightPos-p);
    viewVec = normalize(camPos-p);
    halfVec = normalize(lightVec+viewVec);
    vec3 specular = pow(dot(halfVec,n),specExp)*vec3(1,1,1);
    vec3 diffuse = dot(lightVec,n)*mat;
    return diffuse+ambient+specular;
}

vec3 computeColor(vec3 p, vec3 n, vec3 mat, float rFactor) {
    vec3 lightPos = vec3(4,16,2);
    vec3 ambient = mat*0.1;
    vec3 lightVec;
    vec3 halfVec;
    vec3 viewVec;
    int specExp;

    float t1;
    float t2;
    float t3;
    float t4;
    float t5;
    float t6;
    float t7;
    int shadow;

    float rt1;
    float rt2;
    float rt3;
    float rt4;
    float rt5;
    float rt6;
    float rt7;
    float rtmin = 10001;
    vec3 rp;
    vec3 rn;
    vec3 rcolor;
    vec3 rmat;

    // check for shadow
    t1 = intersectSphere(p,lightPos-p,sphere1Pos,sphere1Radius);
    t2 = intersectSphere(p,lightPos-p,sphere2Pos,sphere2Radius);
    t3 = intersectXZPlane(p,lightPos-p);
    t4 = intersectUpperXZPlane(p,lightPos-p);
    t5 = intersectXYPlane(p,lightPos-p);
    t6 = intersectLeftYZPlane(p,lightPos-p);
    t7 = intersectRightYZPlane(p,lightPos-p);
    if ((t1>0.0001 && t1<1) || (t2>0.0001 && t2<1) || (t3>0.0001 && t3<1))
        shadow = 1;
    else
        shadow = 0;

    if(shadow == 1)
        return ambient;

    // compute reflected color (trace ray for one step)
    vec3 reflectRay = normalize(reflect(normalize(p-camPos),n));
    rt1 = intersectSphere(p, reflectRay, sphere1Pos, sphere1Radius);
    rt2 = intersectSphere(p, reflectRay, sphere2Pos, sphere2Radius);
    rt3 = intersectXZPlane(p, reflectRay);
    rt4 = intersectUpperXZPlane(p,reflectRay);
    rt5 = intersectXYPlane(p,reflectRay);
    rt6 = intersectLeftYZPlane(p,reflectRay);
    rt7 = intersectRightYZPlane(p,reflectRay);
    if (rt1>0.001 && rt1<rtmin) {
        rtmin = rt1;
        rp = p+reflectRay*rt1;
        rn = normalize(rp-sphere1Pos);
        rmat = sphere1Mat;
    }
    if (rt2>0.001 && rt2<rtmin) {
        rtmin = rt2;
        rp = p+reflectRay*rt2;
        rn = normalize(rp-sphere2Pos);
        rmat = sphere2Mat;
    }
    if (rt3>0.001 && rt3<rtmin) {
        rtmin = rt3;
        rp = p+reflectRay*rt3;
        rn = vec3(0,1,0);
        rmat = vec3(0.7,1.0,0.6);
    }
    if (rt4>1 && rt4<rtmin) {
        rn = vec3(0,1,0);
        rtmin = rt4;
        rp = p+reflectRay*rt4;
        rmat = vec3(0.7,1.0,0.6);
    }
    if (rt5>1 && rt5<rtmin) {
        rn = vec3(0,0,1);
        rtmin = rt5;
        rp = p+reflectRay*rt5;
        rmat = vec3(0.6,0.7,1.0);
    }
    if (rt6>1 && rt6<rtmin) {
        rn = vec3(1,0,0);
        rtmin = rt6;
        rp = p+reflectRay*rt6;
        rmat = vec3(1.0,0.6,0.7);
    }
    if (rt7>1 && t7<rtmin) {
        rn = vec3(-1,0,0);
        rtmin = rt7;
        rp = p+reflectRay*rt7;
        rmat = vec3(1.0,0.6,0.7);
    }
    rcolor = computeColorReflect(rp,rn,rmat);

    // compute diffuse and specular
    specExp = 100;
    lightVec = normalize(lightPos-p);
    viewVec = normalize(camPos-p);
    halfVec = normalize(lightVec+viewVec);
    vec3 specular = pow(dot(halfVec,n),specExp)*vec3(1,1,1);
    vec3 diffuse = dot(lightVec,n)*mat;
    return (1-rFactor)*(diffuse+ambient+specular)+rFactor*rcolor;
}

// RNG code taken from https://thebookofshaders.com/10/

float rand(float x) {
    return fract(sin(x)*10);
}
float 2DRand(vec2 v) {
    return fract(sin(dot(v.xy, vec2(12.9898,78.233)))*43758.5453123);
}

vec3 generateRandomUnitVector(vec3 normal) {
    return normalize(vec3(2DRand(normal), rand(normal.z)));
}

vec3 tracePath(vec3 rayo, vec3 rayd, int depth) {
    if(depth > maxDepth) {
        return whitePlastic.ambient;
    }

    float t1;
    float t2;
    float t3;
    float t4;
    float t5;
    float t6;
    float t7;
    float tmin = 10001;

    t1 = intersectSphere(rayo, rayd, sphere1Pos,sphere1Radius);
    t2 = intersectSphere(rayo, rayd, sphere2Pos,sphere2Radius);
    t3 = intersectXZPlane(rayo, rayd);
    t4 = intersectUpperXZPlane(rayo, rayd);
    t5 = intersectXYPlane(rayo, rayd);
    t6 = intersectLeftYZPlane(rayo, rayd);
    t7 = intersectRightYZPlane(rayo, rayd);
    vec3 point;
    vec3 norm;
    
    if (t1>1 && t1<tmin) {
        tmin = t1;
        point = rayo + rayd*tmin;
        norm = normalize(point - sphere1Pos);
    }
    if (t2>1 && t2<tmin) {
        tmin = t2;
        point = rayo + rayd*tmin;
        norm = normalize(point - sphere2Pos);
    }
    if (t3>1 && t3<tmin) {
        norm = vec3(0,1,0);
        tmin = t3;
        point = rayo + rayd*tmin;
    }
    if (t4>1 && t4<tmin) {
        norm = vec3(0,1,0);
        tmin = t4;
        point = rayo + rayd*tmin;
    }
    if (t5>1 && t5<tmin) {
        norm = vec3(0,0,1);
        tmin = t5;
        point = rayo + rayd*tmin;
    }
    if (t6>1 && t6<tmin) {
        norm = vec3(1,0,0);
        tmin = t6;
        point = rayo + rayd*tmin;
    }
    if (t7>1 && t7<tmin) {
        norm = vec3(-1,0,0);
        tmin = t7;
        point = rayo + rayd*tmin;
    }

    vec3 newRayO = point;
    vec3 newRayD = vec3(0); //generate random unit vector

}




void main(void)
{
    // vec3 norm = normalize();
    float t1;
    float t2;
    float t3;
    float t4;
    float t5;
    float t6;
    float t7;
    float tmin = 10001;
    vec3 mat;
    generateRay();
    

    if (tmin<10000)
        fragColor = vec4(computeColor(point, norm, mat, 0.5),1.0);
    else
        fragColor = vec4(0.2, 0.2, 0.2, 1);
}
