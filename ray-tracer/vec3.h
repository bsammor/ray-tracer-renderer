#pragma once
#include <math.h>
#include <iostream>

class vec3
{
private: 
	double x, y, z;
public:
	vec3();
	vec3(double a, double b, double c);
	double get_x();
	double get_y();
	double get_z();
	double magnitude();
	vec3 normalize();

	static double dot_product(vec3 v1, vec3 v2);
	static vec3 cross_product(vec3 v1, vec3 v2);
	static vec3 add(vec3 v1, vec3 v2);
	static vec3 subtract(vec3 v1, vec3 v2);
	static vec3 scalar_mul(vec3 v1, double scalar);
};

