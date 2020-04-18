#pragma once
#include "object.h"
#include <cmath>

class Sphere : public Object
{
private:
	double radius;
	Vec3 position;
public:
	Sphere();
	Sphere(Vec3 position, double radius, Color color, Material material);
	virtual ~Sphere() override {}
	double get_radius();
	virtual bool intersected(Ray* ray, int index);
	Vec3 get_normal(Vec3 point);
};

