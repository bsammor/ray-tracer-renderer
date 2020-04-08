#include "plane.h"

Plane::Plane()
{
	origin = Vec3();
	normal = Vec3();
	color = Color();
}

Plane::Plane(Vec3 origin, Vec3 direction, Color color)
{
	this->origin = origin;
	this->normal = direction;
	this->color = color;
}


bool Plane::intersected(Ray *ray)
{
	// First, check if we intersect
	double dDotN = ray->get_direction().dot_product(normal);

	if (dDotN == 0.0)
	{
		// We just assume the ray is not embedded in the plane
		return false;
	}

	// Find point of intersection
	double t = (origin - ray->get_origin()).dot_product(normal)
		/ dDotN;

	if (t <= ray->get_tnear() || t >= ray->get_tmax())
	{
		// Outside relevant range
		return false;
	}

	ray->set_tmax(t);

	return true;
}

Ray* Plane::create_shadow_ray(Ray* camera_ray, Light light)
{
	return new Ray();
}

Vec3 Plane::get_normal()
{
	return normal;
}
