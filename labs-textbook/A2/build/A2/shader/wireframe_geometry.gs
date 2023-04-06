#version 460 core

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

layout (location = 0) out vec3 gNormal;
layout (location = 1) out vec3 gPosition;
layout (location = 2) out vec2 texCoord;
noperspective out vec3 gEdgeDistance;

layout (location = 0) in vec3 vPos[];
layout (location = 1) in vec3 vNorm[];
layout (location = 2) in vec2 tCoord;




uniform mat4 viewportMatrix; // viewport matrix

void main() {
    // Transform each vertex into viewport space
    vec3 p0 = vec3(viewportMatrix * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec3 p1 = vec3(viewportMatrix * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec3 p2 = vec3(viewportMatrix * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));
    // Find the altitudes (ha, hb and hc)
    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
    float beta = acos( (a*a + c*c - b*b) / (2.0*a*c) );
    float ha = abs( c * sin( beta ) );
    float hb = abs( c * sin( alpha ) );
    float hc = abs( b * sin( alpha ) );
    // Send the triangle along with the edge distances
    gEdgeDistance = vec3( ha, 0, 0 );
    gNormal = vNorm[0];
    gPosition = vPos[0];
    texCoord = tCoord;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    gEdgeDistance = vec3( 0, hb, 0 );
    gNormal = vNorm[1];
    gPosition = vPos[1];
    texCoord = tCoord;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gEdgeDistance = vec3( 0, 0, hc );
    gNormal = vNorm[2];
    gPosition = vPos[2];
    texCoord = tCoord;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}