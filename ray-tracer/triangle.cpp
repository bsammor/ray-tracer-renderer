#include "triangle.h"
#include <iostream>
constexpr double BIAS = 1e-8;

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

bool Triangle::intersected(Ray* ray, int index, double& u, double& v, double& t)
{
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
        return true;
    }

    return false;
}

Vec3 Triangle::get_normal(Vec3 point)
{
    return fn;
}
