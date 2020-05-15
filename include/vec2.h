#ifndef VEC2_H
#define VEC2_H
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>
#define MINIMUM 0.0001f

class Vec2
{
public:
	double x, y;
public:
	Vec2() 
	{
		x = 0.0, y = 0.0;
	}
	Vec2(double a, double b)
	{
		x = a, y = b;
	}
	Vec2 operator + (Vec2& v)
	{
		return Vec2(x + v.x, y + v.y);
	}
	Vec2 operator - (Vec2& v)
	{
		return Vec2(x - v.x, y - v.y);
	}
	Vec2 operator / (double& r)
	{
		return Vec2(x / r, y / r);
	}
	Vec2 operator * (double& r)
	{
		return Vec2(x * r, y * r);
	}
	Vec2& operator /= (double& r)
	{
		x /= r;
		y /= r;
		return *this;
	}
	Vec2& operator *= (double& r)
	{
		x *= r;
		y *= r;
		return *this;
	}
};

#endif
