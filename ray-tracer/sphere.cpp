#include "sphere.h"

Sphere::Sphere()
{
	position = Vec3();
	radius = 0.0;
	color = Color();
	material = diffuse;
}

Sphere::Sphere(Vec3 position, double radius, Color color, Material material)
{
	this->position = position;
	this->radius = radius;
	this->color = color;
	this->material = material;
}

double Sphere::get_radius()
{
	return radius;
}

bool Sphere::intersected(Ray* ray, int index)
{
	Vec3 length = ray->get_origin() - position;

	double a = 1;
	double b = 2 * ray->get_direction().dot_product(length);
	double c = length.dot_product(length) - pow(radius, 2);

	double t0 = (-b - std::sqrt(pow(b, 2) - 4 * a * c)) / (2 * a);
	double t1 = (-b + std::sqrt(pow(b, 2) - 4 * a * c)) / (2 * a);

	if (t0 > ray->get_tnear() && t0 < ray->get_tmax())
	{
		ray->set_tmax(t0);
		ray->set_index(index);
		return true;
	}
	else if (t1 > ray->get_tnear() && t1 < ray->get_tmax())
	{
		ray->set_tmax(t1);
		ray->set_index(index);
		return true;
	}

	return false;
}

Vec3 Sphere::get_normal(Vec3 point)
{
	return (point - position).normalize();
}
