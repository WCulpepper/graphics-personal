#include "icosahedron.h"
#include <vector>

// the X and Z values are for drawing an icosahedron
#define IX 0.525731112119133606 
#define IZ 0.850650808352039932

Icosahedron::Icosahedron(GLfloat size) {
    std::vector<GLfloat> p = {-IX, 0.0, IZ, 
                             IX, 0.0, IZ, 
                             -IX, 0.0, -IZ, 
                             IX, 0.0, -IZ,    
		                     0.0, IZ, IX, 
                             0.0, IZ, -IX, 
                             0.0, -IZ, IX, 
                             0.0, -IZ, -IX,    
		                     IZ, IX, 0.0, 
                             -IZ, IX, 0.0, 
                             IZ, -IX, 0.0, 
                             -IZ, -IX, 0.0};

    std::vector<GLfloat> n = {-IX, 0.0, IZ, 
                             IX, 0.0, IZ, 
                             -IX, 0.0, -IZ, 
                             IX, 0.0, -IZ,    
		                     0.0, IZ, IX, 
                             0.0, IZ, -IX, 
                             0.0, -IZ, IX, 
                             0.0, -IZ, -IX,    
		                     IZ, IX, 0.0, 
                             -IZ, IX, 0.0, 
                             IZ, -IX, 0.0, 
                             -IZ, -IX, 0.0};

    std::vector<GLfloat> tex = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f, 
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.866f
    };

    std::vector<GLuint> el = {0,4,1, 0,9,4, 9,5,4, 4,5,8, 4,8,1,    
		8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3,    
		7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6, 
		6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11};
    
    initBuffers(&el, &p, &n, &tex);
}