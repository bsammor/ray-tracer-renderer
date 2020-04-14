#pragma once
#include <math.h>
#define MINIMUM 0.0001f

class Vec3
{
public: 
	double x, y, z;
public:
	Vec3();
	Vec3(double a, double b, double c);
	double magnitude();
	double squared_magnitude();
	Vec3 normalize();

	double dot_product(Vec3 v1);
	Vec3 cross_product(Vec3 v1);

	Vec3 operator+(Vec3 v1);
	Vec3 operator+=(Vec3 v1);

	Vec3 operator-(Vec3 v1);
	Vec3 operator-=(Vec3 v1);

	Vec3 operator*(double scalar);
	Vec3 operator*(Vec3 v1);
	Vec3 operator*=(double scalar);
	Vec3 operator*=(Vec3 v1);

	Vec3 operator/(double scalar);
	Vec3 operator/(Vec3 v1);
	Vec3 operator/=(double scalar);
	Vec3 operator/=(Vec3 v1);

};

