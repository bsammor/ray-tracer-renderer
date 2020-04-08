#pragma once
#include "light.h"
#include "color.h"
#include "ray.h"
#include <cmath>

class Light
{
private:
	Vec3 origin;
	Color color;
	double intensity;
public:
	Light();
	Light(Vec3 origin, Color color, double intensity);
	Vec3 get_origin();
	Color get_color();
	bool intersected(Ray* ray, Color* pixel, double radius_squared);
	double get_intensity();
	Vec3 get_direction(Vec3 P);
};
	
