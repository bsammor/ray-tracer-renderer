#include "sphere.h"
#include "ray.h"
#include "vec3.h"
#include <iostream>
#include <cmath>
#define RAY_T_MIN 0.0001f
#define RAY_T_MAX 1.0e30f

Sphere::Sphere()
{
	origin = Vec3();
	radius = 0.0;
	color = Vec3();
}

Sphere::Sphere(Vec3 origin, double radius, Vec3 color)
{
	this->origin = origin;
	this->radius = radius;
	this->color = color;
}

double Sphere::get_radius()
{
	return radius;
}

Vec3 Sphere::get_origin()
{
	return origin;
}

Vec3 Sphere::get_color()
{
	return color;
}

double Sphere::get_red()
{
	return color.get_x();
}

double Sphere::get_green()
{
	return color.get_y();
}

double Sphere::get_blue()
{
	return color.get_z();
}

bool Sphere::intersected(Ray ray)
{
	Ray shifted_ray(Vec3::subtract(ray.get_origin(), this->origin), ray.get_direction());

	double a = shifted_ray.get_direction().magnitude() * shifted_ray.get_direction().magnitude();
	double b = Vec3::dot_product(shifted_ray.get_direction(), shifted_ray.get_origin()) * 2;
	double c = (shifted_ray.get_origin().magnitude() * shifted_ray.get_origin().magnitude()) - pow(this->radius,2);

	double result = pow(b,2) - 4 * a * c;

	if (result < 0.0)
	{
		return false;
	}

	double t1 = (-b - std::sqrt(result)) / (2 * a);
	double t2 = (-b + std::sqrt(result)) / (2 * a);

	if (t1 > RAY_T_MIN && t1 < RAY_T_MAX)
	{
		if (t1 < ray.get_intersection())
		{
			ray.set_intersection(t1);
			return true;
		}
	}
	if (t2 > RAY_T_MIN && t2 < RAY_T_MAX)
	{
		if (t2 < ray.get_intersection())
		{
			ray.set_intersection(t2);
			return true;
		}
	}

	return false;

}
