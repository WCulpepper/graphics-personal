Will Culpepper
lab03: CSCI444 Advanced Computer Graphics

1) 576 patches are read in by the file.
2) The computations take place in the tessellation evaluation shader, lines 56-82. The resulting x, y, and z coordinates are put into a vec3 labeled "result" which is then normalized to create the normal vector.
3) We need a normal vector for a variety of reasons, chiefly among them being shading and patch orientation.
4) I can't think of a better way of calculating the normals, in all honesty.

Note: I have some code in here for the icosahedron placement, but I couldn't get it to work. It's in the for loop starting at line 456.