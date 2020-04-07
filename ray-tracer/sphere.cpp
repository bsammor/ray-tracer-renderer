#include "sphere.h"
#include "ray.h"
#include "vec3.h"
#include <algorithm>
#include <iostream>
#include <cmath>
using namespace std;
#define MINIMUM 0.0001f
#define PI 3.14159265358979323846

Sphere::Sphere()
{
	origin = Vec3();
	radius = 0.0;
	color = Vec3();
	albedo = Vec3();
}

Sphere::Sphere(Vec3 origin, double radius, Vec3 color, Vec3 albedo)
{
	this->origin = origin;
	this->radius = radius;
	this->color = color;
	this->albedo = albedo;
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

Ray* Sphere::create_shadow_ray(Vec3 point, Light light)
{
	Vec3 normal = point - origin;
	Vec3 light_dir = light.get_direction(point);
	Ray* ray = new Ray(point + normal * 1e-4, light_dir, MINIMUM, INFINITY);
	return ray;
}
