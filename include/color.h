#ifndef COLOR_H
#define COLOR_H

class Color
{
private:

public:
	Color();
	Color(double r, double g, double b);
	Color(double c);

	Color operator*(Color c1);
	Color operator*(double scalar);

	Color operator/(double scalar);

	Color operator+=(Color c1);
	Color operator+(Color c1);

	double r, g, b;
};

#endif

