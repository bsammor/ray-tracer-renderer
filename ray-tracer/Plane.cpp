#include "plane.h"
#include <iostream>
#include <cmath>
#define MINIMUM 0.0001f
using namespace std;

Plane::Plane()
{
	origin = Vec3();
	direction = Vec3();
	color = Vec3();
}

Plane::Plane(Vec3 origin, Vec3 direction, Vec3 color)
{
	this->origin = origin;
	this->direction = direction;
	this->color = color;
}


bool Plane::intersected(Ray ray)
{
	// First, check if we intersect
	double dDotN = ray.get_direction().dot_product(direction);

	if (dDotN == 0.0)
	{
		// We just assume the ray is not embedded in the plane
		return false;
	}

	// Find point of intersection
	double t = (origin - ray.get_origin()).dot_product(direction)
		/ dDotN;

	if (t <= MINIMUM || t >= INFINITY)
	{
		// Outside relevant range
		return false;
	}

	if (t < ray.get_intersection())
	{
		ray.set_intersection(t);
		return true;
	}

	return false;
}
