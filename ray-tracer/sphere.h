#pragma once
#include "vec3.h"

class sphere
{
private:
	double radius;
	vec3 origin, color;
public:
	sphere();
	sphere(vec3 origin, double radius, vec3 color);
	double get_radius();
	vec3 get_origin();
	vec3 get_color();
	double get_red();
	double get_green();
	double get_blue();
};

