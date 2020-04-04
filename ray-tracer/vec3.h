#pragma once
#include <math.h>
#include <iostream>

class Vec3
{
private: 
	double x, y, z;
public:
	Vec3();
	Vec3(double a, double b, double c);
	double get_x();
	double get_y();
	double get_z();
	double magnitude();
	Vec3 normalize();

	static double dot_product(Vec3 v1, Vec3 v2);
	static Vec3 cross_product(Vec3 v1, Vec3 v2);
	static Vec3 add(Vec3 v1, Vec3 v2);
	static Vec3 subtract(Vec3 v1, Vec3 v2);
	static Vec3 scalar_mul(Vec3 v1, double scalar);
};

