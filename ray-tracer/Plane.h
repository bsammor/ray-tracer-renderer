#pragma once
#include "vec3.h"
#include <math.h>
#include "ray.h"
#include "object.h"

class Plane : public Object
{
private:
	Vec3 direction;
public:
	Plane();
	Plane(Vec3 origin, Vec3 direction, Vec3 color);
	virtual ~Plane() {}
	virtual bool intersected(Ray ray);
};




