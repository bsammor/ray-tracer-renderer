#pragma once
#include "vec3.h"

class ray
{
private:
	vec3 origin, direction;
public:
	ray();
	ray(vec3 origin, vec3 direction);
	vec3 get_origin();
	vec3 get_direction();
};

