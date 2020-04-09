#include "color.h"

Color::Color()
{
	r = 0.0;
	g = 0.0;
	b = 0.0;
}

Color::Color(double r, double g, double b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

double Color::get_r()
{
	return r;
}

double Color::get_g()
{
	return g;
}

double Color::get_b()
{
	return b;
}

Color Color::operator*(Color c1)
{
	return Color(r * c1.get_r(), g * c1.get_g(), b * c1.get_b());
}

Color Color::operator*(double scalar)
{
	return Color(r * scalar, g * scalar, b * scalar);
}

Color Color::operator/(double scalar)
{
	return Color(r / scalar, g / scalar, b / scalar);
}

Color Color::operator+=(Color c1)
{
	r + c1.get_r(), g + c1.get_g(), b + c1.get_b();

	return *this;
}

Color Color::operator+(Color c1)
{
	return Color(r + c1.get_r(), g + c1.get_g(), b + c1.get_b());
}
