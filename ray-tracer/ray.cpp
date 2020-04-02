#include "ray.h"

ray::ray()
{
	origin = vec3();
	direction = vec3();
}

ray::ray(vec3 x, vec3 y)
{
	origin = x;
	direction = y;
}

vec3 ray::get_origin()
{
	return origin;
}

vec3 ray::get_direction()
{
	return direction;
}
