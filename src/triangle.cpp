#include <triangle.h>
#include <iostream>
constexpr double BIAS = 1e-8;
uint64_t numRayTrianglesTests = 0;
uint64_t numRayTrianglesIsect = 0;
uint64_t numPrimaryRays = 0;

Triangle::Triangle()
{
    v0 = Vec3();
    v1 = Vec3(1, 1, 1);
    v2 = Vec3(2, 2, 2);
    color = Color();
    material = diffuse;
    fn = (v1 - v0).cross_product(v2 - v0).normalize();
}

Triangle::Triangle(Vec3 v0, Vec3 v1, Vec3 v2, Color color, Material material)
{
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
    this->color = color;
    this->material = material;
    this->fn = (v1 - v0).cross_product(v2 - v0).normalize();
}

bool Triangle::intersected(std::shared_ptr<Ray> ray, int index, double& u, double& v, double& t)
{
    __sync_fetch_and_add(&numRayTrianglesTests, 1); 
    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    Vec3 pvec = ray->get_direction().cross_product(v0v2);
    float det = v0v1.dot_product(pvec);

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < BIAS) return false;

    float invDet = 1 / det;

    Vec3 tvec = ray->get_origin() - v0;
    u = tvec.dot_product(pvec) * invDet;
    if (u < 0 || u > 1) return false;

    Vec3 qvec = tvec.cross_product(v0v1);
    v = ray->get_direction().dot_product(qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = v0v2.dot_product(qvec) * invDet;

    if (t < ray->get_tmax()) 
    {
        __sync_fetch_and_add(&numRayTrianglesIsect, 1); 
        return true;
    }

    return false;
}

Vec3 Triangle::get_normal(Vec3 point)
{
    return fn;
}

BBOX Triangle::get_bbox() 
{
    BBOX bbox;

    if ( v0.x < bbox.min.x ) bbox.min.x = v0.x;
    if ( v0.y < bbox.min.y ) bbox.min.y = v0.y;
    if ( v0.z < bbox.min.z ) bbox.min.z = v0.z;
    if ( v0.x > bbox.max.x ) bbox.max.x = v0.x;
    if ( v0.y > bbox.max.y ) bbox.max.y = v0.y;
    if ( v0.z > bbox.max.z ) bbox.max.z = v0.z;

    if ( v1.x < bbox.min.x ) bbox.min.x = v1.x;
    if ( v1.y < bbox.min.y ) bbox.min.y = v1.y;
    if ( v1.z < bbox.min.z ) bbox.min.z = v1.z;
    if ( v1.x > bbox.max.x ) bbox.max.x = v1.x;
    if ( v1.y > bbox.max.y ) bbox.max.y = v1.y;
    if ( v1.z > bbox.max.z ) bbox.max.z = v1.z;

    if ( v2.x < bbox.min.x ) bbox.min.x = v2.x;
    if ( v2.y < bbox.min.y ) bbox.min.y = v2.y;
    if ( v2.z < bbox.min.z ) bbox.min.z = v2.z;
    if ( v2.x > bbox.max.x ) bbox.max.x = v2.x;
    if ( v2.y > bbox.max.y ) bbox.max.y = v2.y;
    if ( v2.z > bbox.max.z ) bbox.max.z = v2.z;

    return bbox;
}