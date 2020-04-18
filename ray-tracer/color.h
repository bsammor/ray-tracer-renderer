#pragma once

class Color
{
private:

public:
	Color();
	Color(double r, double g, double b);

	Color operator*(Color c1);
	Color operator*(double scalar);

	Color operator/(double scalar);

	Color operator+=(Color c1);
	Color operator+(Color c1);

	double r, g, b;
};

