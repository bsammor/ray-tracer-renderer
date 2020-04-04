#pragma once
#include "vec3.h"
#include <math.h>
#include "ray.h"

class plane
{
private:
	vec3 position, direction;
public:
	plane();
	plane(vec3 position, vec3 direction);
	bool is_intersected(ray ray);
};


