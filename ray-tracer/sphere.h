#pragma once
#include "ray.h"

class Sphere
{
private:
	double radius;
	Vec3 origin, color;
public:
	Sphere();
	Sphere(Vec3 origin, double radius, Vec3 color);
	double get_radius();
	Vec3 get_origin();
	Vec3 get_color();
	double get_red();
	double get_green();
	double get_blue();
	bool intersected(Ray ray);
};

