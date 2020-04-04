#include "light.h"

Light::Light()
{
	origin = Vec3();
}

Light::Light(Vec3 origin)
{
	this->origin = origin;
}

Vec3 Light::get_origin()
{
	return origin;
}
