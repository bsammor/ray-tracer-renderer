#include "sphere.h"
#include "ray.h"
#include "vec3.h"
#include <iostream>
#include <cmath>
using namespace std;
#define MINIMUM 0.0001f

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

bool Sphere::intersected(Ray ray)
{
	Ray shifted_ray(ray.get_origin() - origin, ray.get_direction());

	double a = shifted_ray.get_direction().magnitude() * shifted_ray.get_direction().magnitude();
	double b = 2 * shifted_ray.get_direction().dot_product(shifted_ray.get_origin());
	double c = (shifted_ray.get_origin().magnitude() * shifted_ray.get_origin().magnitude()) - pow(radius, 2);

	double result = pow(b, 2) - 4 * a * c;

	if (result < 0.0)
	{
		return false;
	}

	double t1 = (-b - std::sqrt(result)) / (2 * a);
	double t2 = (-b + std::sqrt(result)) / (2 * a);

	if (t1 > MINIMUM && t1 < INFINITY)
	{
		if (t1 < ray.get_intersection())
		{
			ray.set_intersection(t1);
			return true;
		}
	}
	if (t2 > MINIMUM && t2 < INFINITY)
	{
		if (t2 < ray.get_intersection())
		{
			ray.set_intersection(t2);
			return true;
		}
	}

	return false;
}
