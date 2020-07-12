#ifndef COLOR_H
#define COLOR_H

class Color
{
public:
	double r, g, b;

	Color();
	Color(double r, double g, double b);
	Color(double c);

	Color operator*(Color c1);
	Color operator*(double scalar);

	Color operator/(double scalar);

	Color operator+=(Color c1);
	Color operator+(Color c1);
};

#endif

