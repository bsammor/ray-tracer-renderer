#include <ray.h>

Ray::Ray()
{
	origin = Vec3();
	direction = Vec3();
	tnear = MINIMUM;
	tmax = INFINITY;
}

Ray::Ray(Vec3 origin, Vec3 direction, double tnear, double tmax)
{
	this->origin = origin;
	this->direction = direction;
	this->tnear = tnear;
	this->tmax = tmax;
}

Vec3 Ray::get_intersection_point()
{
	return origin + (direction * tmax);
}