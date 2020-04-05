#include "vec3.h"
using namespace std;

Vec3::Vec3()
{
	x = 0.0, y = 0.0, z = 0.0;
}

Vec3::Vec3(double x, double y, double z)
{
	this->x = x, this->y = y, this->z = z;
}

double Vec3::get_x()
{
	return x;
}

double Vec3::get_y()
{
	return y;
}

double Vec3::get_z()
{
	return z;
}

double Vec3::magnitude()
{
	return sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0));
}

Vec3 Vec3::normalize()
{
	double length = magnitude();
	return Vec3(x / length, y / length, z / length);
}

double Vec3::dot_product(Vec3 v1)
{
	return (x * v1.get_x()) + (y * v1.get_y()) + (z * v1.get_z());
}

Vec3 Vec3::cross_product(Vec3  v1)
{
	double new_x = (y * v1.get_z()) - (z * v1.get_y());
	double new_y = (z * v1.get_x()) - (x * v1.get_z());
	double new_z = (x * v1.get_y()) - (y * v1.get_x());
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator+(Vec3 v1)
{
	double new_x = x + v1.get_x();
	double new_y = y + v1.get_y();
	double new_z = z + v1.get_z();
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator-(Vec3 v1)
{
	double new_x = x - v1.get_x();
	double new_y = y - v1.get_y();
	double new_z = z - v1.get_z();
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator*(double scalar)
{
	double new_x = x * scalar;
	double new_y = y * scalar;
	double new_z = z * scalar;
	return Vec3(new_x, new_y, new_z);
}
