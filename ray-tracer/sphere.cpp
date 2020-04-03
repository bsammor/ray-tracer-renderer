#include "sphere.h"

sphere::sphere()
{
	origin = vec3();
	radius = 0.0;
	color = vec3();
}

sphere::sphere(vec3 origin, double radius, vec3 color)
{
	this->origin = origin;
	this->radius = radius;
	this->color = color;
}

double sphere::get_radius()
{
	return radius;
}

vec3 sphere::get_origin()
{
	return origin;
}

vec3 sphere::get_color()
{
	return color;
}

double sphere::get_red()
{
	return color.get_x();
}

double sphere::get_green()
{
	return color.get_y();
}

double sphere::get_blue()
{
	return color.get_z();
}

bool sphere::is_intersected()
{
	return false;
}
