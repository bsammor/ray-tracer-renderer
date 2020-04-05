#pragma once
#include "vec3.h"
#include <cmath>
#define MINIMUM 0.0001f

class Ray
{
private:
	Vec3 origin, direction;
	double intersection = INFINITY;
public:
	Ray();
	Ray(Vec3 origin, Vec3 direction);
	Vec3 get_origin();
	void set_intersection(double point);
	double get_intersection();
	Vec3 get_direction();
};



