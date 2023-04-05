Will Culpepper
Assignment 1

Description: 
This first assignment is a demonstration of Phong and Gouraud shading using the Phong Illumination model. Specular calculations can be toggled between Phong and Blinn-Phong.

Usage: 
The 1 and 2 keys toggle between Gouraud and Phong shading, and the 3 and 4 keys toggle between Phong and Blinn-Phong specular/

Compilation instructions: 
In the A1 folder, create the build files with the command cmake -G "MinGW Makefiles" . -B build
In the build folder, use make to compile the program.
In build/A1, run A1.exe

Bugs, implementation details, etc:
- The diffuse lighting "sticks" to the model. That is, the diffuse is calculated once and not updated with the camera's rotation around the scene. I couldn't figure that out, so I'll probably be in office hours soon.
- While there is no mouse control, the camera follows the arcball camera model. See lines 397-400 in A1.cpp
