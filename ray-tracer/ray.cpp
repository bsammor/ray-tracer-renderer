#include "ray.h"

Ray::Ray()
{
	origin = Vec3();
	direction = Vec3();
}

Ray::Ray(Vec3 origin, Vec3 direction)
{
	this->origin = origin;
	this->direction = direction;
}

Vec3 Ray::get_origin()
{
	return origin;
}

void Ray::set_intersection(double point)
{
	this->intersection = point;
}

double Ray::get_intersection()
{
	return this->intersection;
}

Vec3 Ray::get_direction()
{
	return direction;
}