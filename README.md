## A ray tracer done in C++ with the following optimizations:
* CPU Multi-threading
* Bounding Volume Hierarchies using Surface Area Heuristics
* KD-trees using Surface Area Heuristics
* Octrees

# Compiling
The ray tracer can be compiled on a linux system or windows ubuntu subsystem.

**Requires g++ and Makefile**

Type "make" in root directory to compile & build

Type "make clean" to delete the object files

Type "make remove" to delete the binary executable

# Running
To run the base ray tracer with multi-threading only, type "make run"
 
To run the ray tracer with a KD-tree, type "make run-kd"

To run the tracer with a BVH, type "make run-bvh"
 
To run the tracer with an Octree, type "make run-octree"
 
# Resolution
The default resolution of the rendered images is 1920x1080. To select a custom resolution before running the tracer, add res="resolutionhere" to the make run command

Example:
* make run-octree res="640x480"
  
# Multi-threading
The ray tracer runs by default using the maximum number of threads available to the CPU

To specify the number of threads instead, add thr="number" to the make run command

Example: 
* make run-kd thr="7"  

# 3D Models
The tracer's loader can only load the wavefront or .obj 3D model format

Add the model textures in the /textures directory

Place the model and the materials in the /models directory

To specifiy what models to load into the scene, add models="models/modelname.obj" to the make run command. Multiple models can be loaded by adding them with a space inbetween.

Examples:
* make run-bvh models="models/teapot.obj"
* make run-bvh models="models/teapot.obj models/bunny.obj"

# Notes
To adjust the scene (camera, add spheres/planes, lights), open main.cpp and look for the setup_scene() function. There you will find the code to adjust with some information.

To adjust the acceleration structures parameters, open main.cpp and look for the create_accel_structure() function. There you will find information to change them.

The texture coordinates (for mapping) of a model must be between 0 and 1. If a texture uses coordinates outside the range, it will not work properly.

You can use all three extra commands above at the same time.

Adjusting header files will require a make clean.

# Online References
* scratchapixel.com
* pbr-book.org (Phyiscally Based Rendering: From Theory To Implementation)
* flipcode.com/archives/articles.shtml

