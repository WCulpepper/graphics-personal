Discussion of results: Monte Carlo Path Tracing

For this assignment, I chose to work on Monte Carlo Path Tracing, since my final project works with raytracing already.

The main difference between MC Path Tracing and Ray Tracing is that with the MC method, each time a ray bounces it's sent
in a random direction from the point it bounces on (in a hemisphere based on the corresponding normal vector).

My initial thought was to use the GLSL noise1 function to generate a random vector, called on each component of the normal.

I'm missing something important with my implementation, as evidenced by my screenshots comparing my RT to my PT resutls.
I hope that my code demonstrates an understanding of the theory well enough to warrant some partial credit.

To run this code, the standard commands from the semester are needed:

In the A3_Culpepper folder,

cmake -G "MinGW Makefiles" . -B build
cd build
make
cd A3
.\A3.exe