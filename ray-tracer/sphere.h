#pragma once
#include "vec3.h"

class sphere
{
private:
	double radius;
	vec3 position, color;
public:
	sphere();
	sphere(vec3 pos, double radius, vec3 rgb);
	double get_radius();
	vec3 get_position();
	vec3 get_color();
	double get_red();
	double get_green();
	double get_blue();
};

