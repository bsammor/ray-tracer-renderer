#pragma once
#include "vec3.h"
#include <cmath>
#define MINIMUM 0.0001f
#define PI 3.14159265358979323846

class Ray
{
private:
	Vec3 origin, direction;
	double tnear, tmax;
public:
	Ray();
	Ray(Vec3 origin, Vec3 direction, double tnear, double tmax);
	Vec3 get_origin();
	void set_tmax(double point);
	Vec3 get_direction();
	Vec3 get_intersection_point();
	double get_tnear();
	double get_tmax();
};



