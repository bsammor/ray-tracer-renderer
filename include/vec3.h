#ifndef VEC3_H
#define VEC3_H
#include <math.h>

class Vec3
{
public: 
	double x, y, z;

	Vec3();
	Vec3(double a, double b, double c);
	Vec3(double x);
	double magnitude();
	double squared_magnitude();
	Vec3 normalize();

	double dot_product(Vec3 v1);
	Vec3 cross_product(Vec3 v1);

	Vec3 operator+(Vec3 v1);
	Vec3 operator+=(Vec3 v1);

	Vec3 operator-(Vec3 v1);
	Vec3 operator-(Vec3 v1) const;
	Vec3 operator-=(Vec3 v1);

	Vec3 operator*(double scalar);
	Vec3 operator*=(double scalar);

	Vec3 operator*(Vec3 v1);
	Vec3 operator*=(Vec3 v1);

	Vec3 operator/(double scalar);
	Vec3 operator/=(double scalar);

	Vec3 operator/(Vec3 v1);
	Vec3 operator/=(Vec3 v1);

    double operator[](int i) const;
    double &operator[](int i) 
	{
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
};

#endif