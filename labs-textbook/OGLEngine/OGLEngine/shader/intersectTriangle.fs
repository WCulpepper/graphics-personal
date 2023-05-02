double intersectTriangleFast(Ray r, Triangle m)
{
	double  a,b,c,d,e,f,g,h,i,j,k,l;
	double beta,gamma,t;
	
	double eimhf,gfmdi,dhmeg,akmjb,jcmal,blmkc;

	double M;
	
	double dd;
	
	vec3 ma,mb,mc;
	ma = vertices[m.a]; // point 1
	mb = vertices[m.b]; // point 2
	mc = vertices[m.c]; // point 3

	a = ma.x-mb.x; // side 1, p1->p2
	b = ma.y-mb.y;
	c = ma.z-mb.z;

	d = ma.x-mc.x; // side 2, p1->p3
	e = ma.y-mc.y;
	f = ma.z-mc.z;
	
	g = r.b.x; // don't know what this is, I assume it's rayd
	h = r.b.y;
	i = r.b.z;
	
	j = ma.x-r.a.x; // 
	k = ma.y-r.a.y;
	l = ma.z-r.a.z;
	
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


double intersectTriangleSlow(Ray r, Triangle m)
{
	double A,B,C,D; //coefficients for the plane equation
	
	double t;
	
	Vec3 ma,mb,mc;
	ma = vertices[m.a];
	mb = vertices[m.b];
	mc = vertices[m.c];
	int isInside;
	
	Vec3 p,pmc,pma,pmb;
	double dd;
	
	Vec3 c1,c2,c3;
	
	A = m.normal.x;
	B = m.normal.y;
	C = m.normal.z;
	
	D = m.D;
	
	dd = A*r.b.x+B*r.b.y+C*r.b.z;
	if (ABS((dd))<0.0000001) return -1;
	
	t = (-D - A*r.a.x - B*r.a.y - C*r.a.z)/(dd);
	
	p.x = r.a.x+r.b.x*t;
	p.y = r.a.y+r.b.y*t;
	p.z = r.a.z+r.b.z*t;
	
	isInside = 1;
	
	pmc.x = p.x-mc.x; 
	pmc.y = p.y-mc.y; 
	pmc.z = p.z-mc.z;

	pma.x = p.x-ma.x; 
	pma.y = p.y-ma.y; 
	pma.z = p.z-ma.z;
	
	pmb.x = p.x-mb.x; 
	pmb.y = p.y-mb.y; 
	pmb.z = p.z-mb.z;
	
	c1.x = m.AC.y*pmc.z-pmc.y*m.AC.z;
	c1.y = pmc.x*m.AC.z-m.AC.x*pmc.z;
	c1.z = m.AC.x*pmc.y-pmc.x*m.AC.y;
	
	c2.x = m.BA.y*pma.z-pma.y*m.BA.z;
	c2.y = pma.x*m.BA.z-m.BA.x*pma.z;
	c2.z = m.BA.x*pma.y-pma.x*m.BA.y;
	
	c3.x = m.CB.y*pmb.z-pmb.y*m.CB.z;
	c3.y = pmb.x*m.CB.z-m.CB.x*pmb.z;
	c3.z = m.CB.x*pmb.y-pmb.x*m.CB.y;
	
    if (((c1.x>0.000001 && c2.x>0.000001 && c3.x>0.000001) ||	(c1.x<0.000001 && c2.x<0.000001 && c3.x<0.000001)) &&
	    ((c1.y>0.000001 && c2.y>0.000001 && c3.y>0.000001) ||	(c1.y<0.000001 && c2.y<0.000001 && c3.y<0.000001)) &&
		((c1.z>0.000001 && c2.z>0.000001 && c3.z>0.000001) ||	(c1.z<0.000001 && c2.z<0.000001 && c3.z<0.000001)))
				return t;
	else return -1;

}