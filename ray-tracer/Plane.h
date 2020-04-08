#pragma once
#include "object.h"

class Plane : public Object
{
private:
	Vec3 normal;
public:
	Plane();
	Plane(Vec3 origin, Vec3 direction, Color color);
	virtual ~Plane() override {}
	virtual bool intersected(Ray *ray);
	virtual Ray* create_shadow_ray(Ray* camera_ray, Light light);
	Vec3 get_normal();
};




