#pragma once
#include "ray.h"
#include "object.h"

class Sphere : public Object
{
private:
	double radius;
public:
	Sphere();
	Sphere(Vec3 origin, double radius, Vec3 color);
	virtual ~Sphere() {}
	double get_radius();
	virtual bool intersected(Ray ray);
};

