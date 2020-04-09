#pragma once

class Color
{
private:
	double r, g, b;
public:
	Color();
	Color(double r, double g, double b);
	double get_r();
	double get_g();
	double get_b();

	Color operator*(Color c1);
	Color operator*(double scalar);
	Color operator/(double scalar);
	Color operator+=(Color c1);
	Color operator+(Color c1);
};

