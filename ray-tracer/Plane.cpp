#include "Plane.h"
#define RAY_T_MIN 0.0001f
#define RAY_T_MAX 1.0e30f

Plane::Plane()
{
	position = vec3();
	direction = vec3();
}

Plane::Plane(vec3 position, vec3 direction)
{
	this->position = position;
	this->direction = direction;
}

bool Plane::is_intersected(ray ray)
{
	// First, check if we intersect
	double dDotN = vec3::dot_product(ray.get_direction(), direction);

	if (dDotN == 0.0f)
	{
		// We just assume the ray is not embedded in the plane
		return false;
	}

	// Find point of intersection
	float t = vec3::dot_product(vec3::subtract(position, ray.get_origin()), direction) / dDotN;

	if (t <= RAY_T_MIN || t >= RAY_T_MAX)
	{
		// Outside relevant range
		return false;
	}

	return true;
}