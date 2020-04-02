#include "sphere.h"

sphere::sphere()
{
	position = vec3();
	radius = 0.0;
	color = vec3();
}

sphere::sphere(vec3 pos, double r, vec3 rgb)
{
	position = pos;
	radius = 0.0;
	color = rgb;
}

double sphere::get_radius()
{
	return radius;
}

vec3 sphere::get_position()
{
	return position;
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
