#version 460

uniform vec3 camPos;
uniform vec3 camGaze;
uniform vec3 camUp;

in vec2 texCoord;
out vec4 fragColor;

layout(std430, binding=3) buffer Icosahedron {
    vec3[12] vertices;
    vec3[20] indices;
} ico;

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

// intersects a triangle, assuming a counter-clockwise winding of the points.
//    B
// C /\ A


// float intersectTriangle(vec3 rayo, vec3 rayd, vec3 p1, vec3 p2, vec3 p3) {
//     vec3 sideBA = p2 - p1;
//     vec3 sideCA = p3 - p1;
//     vec3 pointToTri = rayo - p1;

//     float d = 0;
// }

float intersectTriangle(vec3 rayo, vec3 rayd, vec3 p1, vec3 p2, vec3 p3) {
    float  a,b,c,d,e,f,g,h,i,j,k,l;
	float beta,gamma,t;
	
	float eimhf,gfmdi,dhmeg,akmjb,jcmal,blmkc;

	float M;
	
	float dd;
	
	vec3 ma,mb,mc;
	ma = p1; // point 1
	mb = p2; // point 2
	mc = p3; // point 3

	a = ma.x-mb.x; // side 1, p1->p2
	b = ma.y-mb.y;
	c = ma.z-mb.z;

	d = ma.x-mc.x; // side 2, p1->p3
	e = ma.y-mc.y;
	f = ma.z-mc.z;
	
	g = rayd.x; // 
	h = rayd.y;
	i = rayd.z;
	
	j = ma.x-rayo.x; // 
	k = ma.y-rayo.y;
	l = ma.z-rayo.z;
	
	eimhf = e*i-h*f;
	gfmdi = g*f-d*i;
	dhmeg = d*h-e*g;
	akmjb = a*k-j*b;
	jcmal = j*c-a*l;
	blmkc = b*l-k*c;

	M = a*eimhf+b*gfmdi+c*dhmeg;
	
	t = -(f*akmjb+e*jcmal+d*blmkc)/M;
	
	if (t<0) return -1;
	
	gamma = (i*akmjb+h*jcmal+g*blmkc)/M;
	
	if (gamma<0 || gamma>1) return -1;
	
	beta = (j*eimhf+k*gfmdi+l*dhmeg)/M;
	
	if (beta<0 || beta>(1-gamma)) return -1;
	
	return t;
}

float intersectSphere(vec3 rayo, vec3 rayd, vec3 center, float r) {
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

vec3 computeColorReflect(vec3 p, vec3 n, vec3 mat, int i) {
    
    vec3 lightPos = vec3(4,16,2);
    vec3 ambient = mat*0.1;
    vec3 lightVec;
    vec3 halfVec;
    vec3 viewVec;
    int specExp;

    int ia = int(ico.indices[i].x);
    int ib = int(ico.indices[i].y);
    int ic = int(ico.indices[i].z);

    vec3 a = ico.vertices[ia];
    vec3 b = ico.vertices[ib];
    vec3 c = ico.vertices[ic];

    float t1;
    float t2;
    float t3;
    int shadow;

    // check for shadow
    t1 = intersectTriangle(p,lightPos-p, a, b, c);
    t3 = intersectXZPlane(p,lightPos-p);
    if ((t1>0.0001 && t1<1) || (t3>0.0001 && t3<1))
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

vec3 computeColor(vec3 p, vec3 n, vec3 mat, float rFactor, int i) {

    int ia = int(ico.indices[i].x);
    int ib = int(ico.indices[i].y);
    int ic = int(ico.indices[i].z);

    vec3 a = ico.vertices[ia];
    vec3 b = ico.vertices[ib];
    vec3 c = ico.vertices[ic];

    vec3 lightPos = vec3(4,16,2);
    vec3 ambient = mat*0.1;
    vec3 lightVec;
    vec3 halfVec;
    vec3 viewVec;
    int specExp;

    float t1;
    float t2;
    float t3;
    int shadow;

    float rt1;
    float rt2;
    float rt3;
    float rtmin = 10001;
    vec3 rp;
    vec3 rn;
    vec3 rcolor;
    vec3 rmat;

    // check for shadow
    t1 = intersectTriangle(p,lightPos-p, a, b, c);
    t3 = intersectXZPlane(p,lightPos-p);
    if ((t1>0.0001 && t1<1) || (t2>0.0001 && t2<1) || (t3>0.0001 && t3<1))
        shadow = 1;
    else
        shadow = 0;

    if(shadow == 1)
        return ambient;

    // compute reflected color (trace ray for one step)
    vec3 reflectRay = normalize(reflect(normalize(p-camPos),n));
    rt1 = intersectTriangle(p, reflectRay, a, b, c);
    rt3 = intersectXZPlane(p, reflectRay);
    if (rt1>0.001 && rt1<rtmin) {
        vec3 n1 = b-a;
        vec3 n2 = c-a;
        rtmin = rt1;
        rp = p+reflectRay*rt1;
        rn = normalize(cross(n1, n2));
        rmat = vec3(1.0, 0.0, 0.0);
    }
    if (rt3>0.001 && rt3<rtmin) {
        rtmin = rt3;
        rp = p+reflectRay*rt3;
        rn = vec3(0,1,0);
        rmat = vec3(0.6,1.0,0.7);
    }
    rcolor = computeColorReflect(rp,rn,rmat, i);

    // compute diffuse and specular
    specExp = 100;
    lightVec = normalize(lightPos-p);
    viewVec = normalize(camPos-p);
    halfVec = normalize(lightVec+viewVec);
    vec3 specular = pow(dot(halfVec,n),specExp)*vec3(1,1,1);
    vec3 diffuse = dot(lightVec,n)*mat;
    return (1-rFactor)*(diffuse+ambient+specular)+rFactor*rcolor;
}

void main(void)
{
    // vec3 norm = normalize();
    int index;
    float t1;
    float t3;
    float tmin = 10001;
    vec3 mat;
    vec3 point;
    vec3 norm;
    generateRay();

    for(int i = 0; i < 20; i++) {
        int ia = int(ico.indices[i].x);
        int ib = int(ico.indices[i].y);
        int ic = int(ico.indices[i].z);

        vec3 a = ico.vertices[ia];
        vec3 b = ico.vertices[ib];
        vec3 c = ico.vertices[ic];
        t1 = intersectTriangle(rayo, rayd, a, b, c);
        t3 = intersectXZPlane(rayo, rayd);
        if (t1>1 && t1<tmin) {
            tmin = t1;
            point = rayo + rayd*tmin;
            vec3 n1 = b-a;
            vec3 n2 = c-a;
            norm = normalize(cross(n1, n2));
            mat = vec3(1.0, 0.0, 0.0);
            index = i;
        }

        if (t3>1 && t3<tmin) {
            norm = vec3(0,1,0);
            tmin = t3;
            point = rayo + rayd*tmin;
            mat = vec3(0.6,1.0,0.7);
            index = -1;
        }
    }

    if (tmin<10000)
        fragColor = vec4(computeColor(point, norm, mat, 0.5, index),1.0);
    else
        fragColor = vec4(0.2, 0.2, 0.2, 1);
}
