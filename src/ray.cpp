#include <ray.h>
#include <iostream>

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

void Ray::set_tmax(double z)
{
	tmax = z;
}

void Ray::set_index(int i)
{
	obj_index = i;
}

int Ray::get_index()
{
	return obj_index;
}

Vec3 Ray::get_intersection_point()
{
	return origin + (direction * tmax);
}

double Ray::get_tnear()
{
	return tnear;
}