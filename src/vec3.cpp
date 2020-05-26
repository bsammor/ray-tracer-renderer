#include <vec3.h>

Vec3::Vec3()
{
	x = 0.0, y = 0.0, z = 0.0;
}

Vec3::Vec3(double x, double y, double z)
{
	this->x = x, this->y = y, this->z = z;
}

double Vec3::magnitude()
{
	return sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0));
}

double Vec3::squared_magnitude()
{
	return (magnitude() * magnitude());
}

Vec3 Vec3::normalize()
{
	double length = magnitude();
	return Vec3(x / length, y / length, z / length);
}

double Vec3::dot_product(Vec3 v1)
{
	return (x * v1.x) + (y * v1.y) + (z * v1.z);
}

Vec3 Vec3::cross_product(Vec3 v1)
{
	double new_x = (y * v1.z) - (z * v1.y);
	double new_y = (z * v1.x) - (x * v1.z);
	double new_z = (x * v1.y) - (y * v1.x);
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator*(Vec3  v1)
{
	double new_x = x * v1.x;
	double new_y = y * v1.y;
	double new_z = z * v1.z;
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator*=(double scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

Vec3 Vec3::operator*=(Vec3 v1)
{
	x *= v1.x;
	y *= v1.y;
	z *= v1.z;
	return *this;
}

Vec3 Vec3::operator+(Vec3 v1)
{
	double new_x = x + v1.x;
	double new_y = y + v1.y;
	double new_z = z + v1.z;
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator+=(Vec3 v1)
{
	x += v1.x;
	y += v1.y;
	z += v1.z;
	return *this;
}

Vec3 Vec3::operator-(Vec3 v1)
{
	double new_x = x - v1.x;
	double new_y = y - v1.y;
	double new_z = z - v1.z;
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator-(Vec3 v1) const
{
	double new_x = x - v1.x;
	double new_y = y - v1.y;
	double new_z = z - v1.z;
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator-=(Vec3 v1)
{
	x -= v1.x;
	y -= v1.y;
	z -= v1.z;
	return *this;
}

Vec3 Vec3::operator*(double scalar)
{
	double new_x = x * scalar;
	double new_y = y * scalar;
	double new_z = z * scalar;
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator/(double scalar)
{
	double new_x = x / scalar;
	double new_y = y / scalar;
	double new_z = z / scalar;
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator/(Vec3 v1)
{
	double new_x = x / v1.x;
	double new_y = y / v1.y;
	double new_z = z / v1.z;
	return Vec3(new_x, new_y, new_z);
}

Vec3 Vec3::operator/=(double scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}