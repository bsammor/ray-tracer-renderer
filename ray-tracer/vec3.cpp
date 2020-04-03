#include "vec3.h"
using namespace std;

vec3::vec3()
{
	x = 0.0, y = 0.0, z = 0.0;
}

vec3::vec3(double x, double y, double z)
{
	this->x = x, this->y = y, this->z = z;
}

double vec3::get_x()
{
	return x;
}

double vec3::get_y()
{
	return y;
}

double vec3::get_z()
{
	return z;
}

double vec3::magnitude()
{
	return sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0));
}

vec3 vec3::normalize()
{
	double length = magnitude();
	return vec3(x / length, y / length, z / length);
}

double vec3::dot_product(vec3 v1, vec3  v2)
{
	return (v1.get_x() * v2.get_x()) + (v1.get_y() * v2.get_y()) + (v1.get_z() * v2.get_z());
}

vec3 vec3::cross_product(vec3  v1, vec3  v2)
{
	double new_x = (v1.get_y() * v2.get_z()) - (v1.get_z() * v2.get_y());
	double new_y = (v1.get_z() * v2.get_x()) - (v1.get_x() * v2.get_z());
	double new_z = (v1.get_x() * v2.get_y()) - (v1.get_y() * v2.get_x());
	return vec3(new_x, new_y, new_z);
}


vec3 vec3::add(vec3  v1, vec3  v2)
{
	double new_x = v1.get_x() + v2.get_x();
	double new_y = v1.get_y() + v2.get_y();
	double new_z = v1.get_z() + v2.get_z();
	return vec3(new_x, new_y, new_z);
}

vec3 vec3::subtract(vec3  v1, vec3  v2)
{
	double new_x = v1.get_x() - v2.get_x();
	double new_y = v1.get_y() - v2.get_y();
	double new_z = v1.get_z() - v2.get_z();
	return vec3(new_x, new_y, new_z);
}

vec3 vec3::scalar_mul(vec3  v1, double scalar)
{
	double new_x = v1.get_x() * scalar;
	double new_y = v1.get_y() * scalar;
	double new_z = v1.get_z() * scalar;
	return vec3(new_x, new_y, new_z);
}
