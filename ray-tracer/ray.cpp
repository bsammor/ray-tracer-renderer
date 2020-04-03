#include "ray.h"

ray::ray()
{
	origin = vec3();
	direction = vec3();
}

ray::ray(vec3 origin, vec3 direction)
{
	this->origin = origin;
	this->direction = direction;
}

vec3 ray::get_origin()
{
	return origin;
}

vec3 ray::get_direction()
{
	return direction;
}
