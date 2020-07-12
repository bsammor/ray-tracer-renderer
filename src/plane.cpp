#include <plane.h>

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

bool Plane::intersected(std::shared_ptr<Ray> ray, int index)
{
	double t = 0;
	double dDotN = ray->get_direction().dot_product(normal);

	if (dDotN == 0.0) return false;
	t = (position - ray->get_origin()).dot_product(normal) / dDotN;

	if (t <= ray->tnear || t >= ray->tmax) return false;

	ray->tmax = t;
	ray->obj_index = index;

	return true;
}

Vec3 Plane::get_normal(Vec3 point)
{
	return normal;
}

BBOX Plane::get_bbox() 
{
	return BBOX();
}