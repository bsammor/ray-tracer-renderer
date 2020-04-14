#pragma once
#include "object.h"
#include <vector>
#include "vec3.h"
#include <memory>

class TriangleMesh : public Object
{
public:
    uint32_t nfaces;
    uint32_t* vertsIndex;
    uint32_t* faceIndex;
    Vec3* verts;
    Vec3* normals;
    Vec3* st;

public: 
    // Build a triangle mesh from a face index array and a vertex index array
    TriangleMesh(
        uint32_t nfaces,
        uint32_t* faceIndex,
        uint32_t* vertsIndex,
        Vec3* verts,
        Vec3* normals,
        Vec3* st);
    virtual bool intersected(Ray* ray, int index);
    virtual Ray* create_shadow_ray(Ray* camera_ray, Light light);
    virtual Vec3 get_normal(Vec3 point);a
};

