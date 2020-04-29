#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H
#include <iostream>
#include "tiny_obj_loader.h"
#include "vec3.h"
#include "vec2.h"
#include "triangle.h"
#define TINYOBJLOADER_IMPLEMENTATION 

class TriangleMesh : public Object
{
public:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    Triangle tri;
    Vec3 tri_tex_coordinates;
    Vec3 tri_vnormal;
    Vec3 tri_fnormal;
    std::string warn;
    std::string err;
    std::string tex_name;

public: 
    TriangleMesh(const char* file, Color color, Material material);
    virtual bool intersected(Ray* ray, int index, double& u, double& v, double& tmax);
    virtual Vec3 get_normal(Vec3 point);
};

#endif

