#pragma once
#include "vec3.h"
#define RAY_T_MIN 0.0001f
#define RAY_T_MAX 1.0e30f

class Ray
{
private:
	Vec3 origin, direction;
	double intersection = RAY_T_MAX;
public:
	Ray();
	Ray(Vec3 origin, Vec3 direction);
	Vec3 get_origin();
	void set_intersection(double point);
	double get_intersection();
	Vec3 get_direction();
};



