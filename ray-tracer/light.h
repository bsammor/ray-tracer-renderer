#pragma once
#include "vec3.h"

class light
{
private:
	vec3 origin;
public:
	light();
	light(vec3 origin);
	vec3 get_origin();
};
	
