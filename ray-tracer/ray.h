#pragma once
#include "vec3.h"

class ray
{
private:
	vec3 origin, direction;
public:
	ray();
	ray(vec3 x, vec3 y);
	vec3 get_origin();
	vec3 get_direction();
};

