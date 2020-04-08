#include "sphere.h"

Sphere::Sphere()
{
	origin = Vec3();
	radius = 0.0;
	color = Color();
}

Sphere::Sphere(Vec3 origin, double radius, Color color)
{
	this->origin = origin;
	this->radius = radius;
	this->color = color;
}

double Sphere::get_radius()
{
	return radius;
}

bool Sphere::intersected(Ray *ray)
{
	Vec3 length = ray->get_origin() - origin;

	double a = 1;
	double b = 2 * ray->get_direction().dot_product(length);
	double c = length.dot_product(length) - pow(radius, 2);

	double t0 = (-b - std::sqrt(pow(b, 2) - 4 * a * c)) / (2 * a);
	double t1 = (-b + std::sqrt(pow(b, 2) - 4 * a * c)) / (2 * a);

	if (t0 > ray->get_tnear() && t0 < ray->get_tmax())
	{
		ray->set_tmax(t0);
		return true;
	}
	else if (t1 > ray->get_tnear() && t1 < ray->get_tmax())
	{
		ray->set_tmax(t1);
		return true;
	}

	return false;
}

Ray* Sphere::create_shadow_ray(Ray* camera_ray, Light light, Color *light_intensity)
{
	Vec3 point = camera_ray->get_intersection_point();
	Vec3 normal = (point - origin).normalize();
	Vec3 light_dir = point - light.get_origin();
	double shadow_bias = 1e-4;
	double radius_squared = light_dir.dot_product(light_dir);
	double dist = sqrtl(radius_squared);

	Ray* ray = new Ray(point + normal, light_dir.normalize(), MINIMUM, dist);
	return ray;
}
