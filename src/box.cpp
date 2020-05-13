#include <box.h>
#include <iostream>
#include <limits>

Box::Box() {
    double min_num = std::numeric_limits<double>::lowest();
    double max_num = std::numeric_limits<double>::max();
    min = Vec3(max_num, max_num, max_num);
    max = Vec3(min_num, min_num, min_num);
}

Box::Box(Vec3 min, Vec3 max) 
{
    this->min = min;
    this->max = max;
}


bool Box::intersected(Ray* ray)
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
