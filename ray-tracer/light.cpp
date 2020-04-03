#include "light.h"

light::light()
{
	origin = vec3();
}

light::light(vec3 origin)
{
	this->origin = origin;
}

vec3 light::get_origin()
{
	return origin;
}
