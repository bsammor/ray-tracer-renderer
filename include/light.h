// Theory and Implementation insipred by 
// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/shading-spherical-light
#ifndef LIGHT_H
#define LIGHT_H
#include <ray.h>
#include <memory>

class Light
{
private:
	Vec3 position;
	Color color;
	double intensity;

public:
	Light();
	Light(Vec3 position, Color color, double intensity);
	Vec3 get_position();
	Color get_color();
	bool intersected(std::shared_ptr<Ray> ray, std::shared_ptr<Color> pixel, double radius_squared);
	double get_intensity();
	Vec3 get_direction(Vec3 P);
};
	
#endif