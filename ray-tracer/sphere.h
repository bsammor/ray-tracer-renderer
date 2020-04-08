#pragma once
#include "object.h"
#include <cmath>

class Sphere : public Object
{
private:
	double radius;
public:
	Sphere();
	Sphere(Vec3 origin, double radius, Color color);
	virtual ~Sphere() override {}
	double get_radius();
	virtual bool intersected(Ray *ray);
	virtual Ray* create_shadow_ray(Ray *camera_ray, Light light);
};

