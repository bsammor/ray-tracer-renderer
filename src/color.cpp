#include <color.h>

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

Color Color::operator*(Color c1)
{
	return Color(r * c1.r, g * c1.g, b * c1.b);
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
	r += c1.r;
	g += c1.g;
	b += c1.b;

	return *this;
}

Color Color::operator+(Color c1)
{
	return Color(r + c1.r, g + c1.g, b + c1.b);
}
