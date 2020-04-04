#pragma once
#include "vec3.h"
#include <math.h>
#include "ray.h"

class Plane
{
private:
	Vec3 position, direction;
public:
	Plane();
	Plane(Vec3 position, Vec3 direction);
	bool intersected(Ray ray);
};




