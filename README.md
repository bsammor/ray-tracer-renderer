## A ray tracer done in C++ with the following optimizations:
* CPU Multi-threading
* Bounding Boxes
* Bounding Volume Hierarchies using Surface Area Heuristics
* KD-trees using Surface Area Heuristics
* Octrees

# Compiling
The ray tracer can be compiled on a linux system or windows ubuntu subsystem.

Requires g++ and Makefile

Type "make" in root directory to compile & build

# Running
 To run the default ray tracer with multi-threading and simple bounding boxes only, type "make run" or ./tracer
 
 To run the ray tracer with a KD-tree, type "make run-kd" or ./tracer kd
 
 To run the tracer with a BVH, type "make run-bvh" or ./tracer bvh
 
 To run the tracer with an Octree, type "make run-octree" or ./tracer octree
 
# Multi-threading
The ray tracer runs by default using the maximum number of threads available to the CPU.

To specify the number of threads instead, add args="number here" to the make run command or add the number as a second argument to ./tracer

Examples: 
* make run-kd args="7"  
* ./tracer kd 7
 
# Triangle Meshes
The tracer's loader can only load the wavefront or .obj 3d model format (can be adjusted by changing the used loader tiny_obj_loader.h)

Add the required textures in the /textures directory

Place the model and the materials in the models directory

