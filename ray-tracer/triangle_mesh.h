#pragma once
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

    std::string warn;
    std::string err;

public: 
    TriangleMesh(const char* file);
    virtual bool intersected(Ray* ray, int index);
    virtual Vec3 get_normal(Vec3 point);
};

