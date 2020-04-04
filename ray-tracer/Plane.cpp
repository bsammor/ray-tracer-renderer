#include "plane.h"
#include <iostream>
#define RAY_T_MIN 0.0001f
#define RAY_T_MAX 1.0e30f
using namespace std;

Plane::Plane()
{
	position = Vec3();
	direction = Vec3();
}

Plane::Plane(Vec3 position, Vec3 direction)
{
	this->position = position;
	this->direction = direction;
}


bool Plane::intersected(Ray ray)
{
	// First, check if we intersect
	double dDotN = Vec3::dot_product(ray.get_direction(), this->direction);

	if (dDotN == 0.0)
	{
		// We just assume the ray is not embedded in the plane
		return false;
	}

	// Find point of intersection
	double t = Vec3::dot_product(Vec3::subtract(this->position, ray.get_origin()), this->direction)
		/ dDotN;

	if (t <= RAY_T_MIN || t >= RAY_T_MAX)
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