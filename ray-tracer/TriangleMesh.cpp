#include "TriangleMesh.h"

TriangleMesh::TriangleMesh(uint32_t nfaces, uint32_t* faceIndex, uint32_t* vertsIndex, Vec3* verts, Vec3* normals, Vec3* st)
{
	this->nfaces = nfaces;
	this->faceIndex = faceIndex;
	this->vertsIndex = vertsIndex;
	this->verts = verts;
	this->normals = normals;
	this->st = st;
}

triangle_intersection(Ray* ray, int index)
{
    float t = 0.0;
    float u = 0.0, v = 0.0;
    // compute plane's normal

    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    // no need to normalize
    Vec3 N = v0v1.cross_product(v0v2); // N 
    float denom = N.dot_product(N);
    //std::cout << denom << std::endl;
    // Step 1: finding P

    // check if ray and plane are parallel ?
    float NdotRayDirection = N.dot_product(ray->get_direction());
    //std::cout << NdotRayDirection << std::endl;
    if (fabs(NdotRayDirection) < 1e-8) // almost 0 
        return false; // they are parallel so they don't intersect ! 
    // compute d parameter using equation 2
    float d = N.dot_product(v0);
    //std::cout << d << std::endl;

    // compute t (equation 3)
    t = (N.dot_product(ray->get_origin()) + d) / NdotRayDirection;
    //std::cout << t << std::endl;
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind 
    // compute the intersection point using equation 1
    Vec3 P = ray->get_origin() + (ray->get_direction() * t);

    // Step 2: inside-outside test
    Vec3 C; // vector perpendicular to triangle's plane 

    // edge 0
    Vec3 edge0 = v1 - v0;
    Vec3 vp0 = P - v0;
    C = edge0.cross_product(vp0);
    if (N.dot_product(C) < 0) return false; // P is on the right side 
    // edge 1
    Vec3 edge1 = v2 - v1;
    Vec3 vp1 = P - v1;
    C = edge1.cross_product(vp1);
    if ((u = N.dot_product(C)) < 0)  return false; // P is on the right side 
    // edge 2
    Vec3 edge2 = v0 - v2;
    Vec3 vp2 = P - v2;
    C = edge2.cross_product(vp2);
    if ((v = N.dot_product(C)) < 0) return false; // P is on the right side; 
    u /= denom;
    v /= denom;

    ray->set_index(index);
    //std::cout << "yes";
    return true; // this ray hits the triangle 
}


bool TriangleMesh::intersected(Ray* ray, int index)
{
    uint32_t j = 0;
    bool isect = false;
    for (uint32_t i = 0; i < nfaces; ++i) {
        const Vec3& v0 = P[trisIndex[j]];
        const Vec3& v1 = P[trisIndex[j + 1]];
        const Vec3& v2 = P[trisIndex[j + 2]];
        float u, v;
        if (triangle_intersection(ray->get_origin(), ray->get_direction(), v0, v1, v2, t, u, v) && t < tNear) {
            tNear = t;
            uv.x = u;
            uv.y = v;
            triIndex = i;
            ray->set_index(index);
            isect = true;
        }
        j += 3;
    }

    return isect;
}

Ray* TriangleMesh::create_shadow_ray(Ray* camera_ray, Light light)
{
	return nullptr;
}

Vec3 TriangleMesh::get_normal(Vec3 point)
{
	return Vec3();
}

uint32_t TriangleMesh::get_nfaces()
{
	return nfaces;
}
