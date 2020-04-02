#pragma once
#include "vec3.h"

class camera
{
private:
	vec3 position, direction;
public:
	camera();
	camera(vec3 pos, vec3 dir);
};

