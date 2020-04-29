#include "plane.h"

Plane::Plane()
{
	position = Vec3();
	normal = Vec3();
	color = Color();
	material = diffuse;
}

Plane::Plane(Vec3 position, Vec3 direction, Color color, Material material)
{
	this->position = position;
	this->normal = direction;
	this->color = color;
	this->material = material;
}


bool Plane::intersected(Ray* ray, int index, double &u, double &v, double& t)
{
	// First, check if we intersect
	double dDotN = ray->get_direction().dot_product(normal);

	if (dDotN == 0.0)
	{
		// We just assume the ray is not embedded in the plane
		return false;
	}

	// Find point of intersection
	t = (position - ray->get_origin()).dot_product(normal)
		/ dDotN;

	if (t <= ray->get_tnear() || t >= ray->get_tmax())
	{
		// Outside relevant range
		return false;
	}

	return true;
}

Vec3 Plane::get_normal(Vec3 point)
{
	return normal;
}
