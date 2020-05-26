## A ray tracer done in C++ with the following optimizations:
* CPU Multi-threading
* Bounding Volume Hierarchies using Surface Area Heuristics
* Octrees
* KD-trees

# Compiling
Requires g++ and Makefile

Type "make" in root directory to compile & build

# Running
 To run the ray tracer, type "make run" or ./tracer

# Triangle Meshes
The tracer's loader can only load the wavefront or .obj 3d model format (can be adjusted by changing the used loader tiny_obj_loader.h)

Add the required textures in the /textures directory

Place the model and the materials in the root directory

