#pragma once
#include "ray.h"
#include "light.h"
#include "object.h"

class Sphere : public Object
{
private:
	double radius;
public:
	Sphere();
	Sphere(Vec3 origin, double radius, Vec3 color, Vec3 albedo);
	virtual ~Sphere() override {}
	double get_radius();
	virtual bool intersected(Ray *ray);
	virtual Ray* create_shadow_ray(Vec3 point, Light light);
};

