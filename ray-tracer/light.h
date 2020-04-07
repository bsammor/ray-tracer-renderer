#pragma once
#include "light.h"
#include "ray.h"

class Light
{
private:
	Vec3 origin, color;
	double intensity;
public:
	Light();
	Light(Vec3 origin, Vec3 color, double intensity, double radius);
	Vec3 get_origin();
	Vec3 get_color();
	double get_intensity();
	bool intersected(Ray *ray);
	Vec3 get_direction(Vec3 P);
};
	
