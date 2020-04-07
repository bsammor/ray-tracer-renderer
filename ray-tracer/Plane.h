#pragma once
#include "vec3.h"
#include <math.h>
#include "ray.h"
#include "object.h"

class Plane : public Object
{
private:
	Vec3 normal;
public:
	Plane();
	Plane(Vec3 origin, Vec3 direction, Vec3 color, Vec3 albedo);
	virtual ~Plane() override {}
	virtual bool intersected(Ray *ray);
	virtual Ray* create_shadow_ray(Vec3 point, Light light);
	Vec3 get_normal();
};




