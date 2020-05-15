#include <bbox.h>
#include <iostream>
#include <limits>

BBOX::BBOX() 
{
    double min_num = std::numeric_limits<double>::lowest();
    double max_num = std::numeric_limits<double>::max();
    min = Vec3(max_num, max_num, max_num);
    max = Vec3(min_num, min_num, min_num);
}

BBOX::BBOX(Vec3 min, Vec3 max) 
{
    this->min = min;
    this->max = max;
}

bool BBOX::intersected(std::shared_ptr<Ray> ray)
{ 
    float tmin = (min.x - ray->get_origin().x) / ray->get_direction().x; 
    float tmax = (max.x - ray->get_origin().x) / ray->get_direction().x; 
 
    if (tmin > tmax) std::swap(tmin, tmax); 
 
    float tymin = (min.y - ray->get_origin().y) / ray->get_direction().y; 
    float tymax = (max.y - ray->get_origin().y) / ray->get_direction().y; 
 
    if (tymin > tymax) std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (min.z - ray->get_origin().z) / ray->get_direction().z; 
    float tzmax = (max.z - ray->get_origin().z) / ray->get_direction().z; 
 
    if (tzmin > tzmax) std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
    return true; 
} 

Vec3 BBOX::get_center()
{
    return (max + min) * 0.5;
}

BBOX BBOX::union_bbox(BBOX b1, BBOX b2) 
{
    BBOX b3;
    b3.min = Vec3(std::min(b1.min.x, b2.min.x), std::fmin(b1.min.y, b2.min.y), std::fmin(b1.min.z, b2.min.z));
    b3.max = Vec3(std::max(b1.max.x, b2.max.x), std::fmax(b1.max.y, b2.max.y), std::fmax(b1.max.z, b2.max.z));

    return b3;
}

BBOX BBOX::union_bbox(BBOX b1, Vec3 p) 
{
    BBOX b3;
    b3.min = Vec3(std::min(b1.min.x, p.x), std::fmin(b1.min.y, p.y), std::fmin(b1.min.z, p.z));
    b3.max = Vec3(std::max(b1.max.x, p.x), std::fmax(b1.max.y, p.y), std::fmax(b1.max.z, p.z));

    return b3;
}

Vec3 BBOX::diagonal() 
{ 
    return max - min; 
}

int BBOX::maximum_extent() 
{
    Vec3 d = diagonal();
    if (d.x > d.y && d.x > d.z)
        return 0;
    else if (d.y > d.z)
        return 1;
    else
        return 2;
}

Vec3 BBOX::offset(Vec3 &p) const 
{
    Vec3 o = p - min;
    if (max.x > min.x) o.x /= max.x - min.x;
    if (max.y > min.y) o.y /= max.y - min.y;
    if (max.z > min.z) o.z /= max.z - min.z;
    return o;
}

double BBOX::surface_area()
{
    Vec3 d = diagonal();
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}