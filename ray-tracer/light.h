#pragma once
#include "vec3.h"

class Light
{
private:
	Vec3 origin;
public:
	Light();
	Light(Vec3 origin);
	Vec3 get_origin();
};
	
