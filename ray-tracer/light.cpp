#include "light.h"

Light::Light()
{
	origin = Vec3();
	intensity = 0.0;
	color = Vec3();
}

Light::Light(Vec3 origin, Vec3 color, double intensity, double radius)
{
	this->origin = origin;
	this->intensity = intensity;
	this->color = color;
}

Vec3 Light::get_origin()
{
	return origin;
}

Vec3 Light::get_color()
{
	return color;
}

double Light::get_intensity()
{
	return intensity;
}

bool Light::intersected(Ray *ray)
{
	return false;
}

Vec3 Light::get_direction(Vec3 P)
{
	return (origin - P);
}
