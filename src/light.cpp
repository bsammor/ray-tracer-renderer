#include <light.h>

Light::Light()
{
	position = Vec3();
	intensity = 0.0;
	color = Color();
}

Light::Light(Vec3 position, Color color, double intensity)
{
	this->position = position;
	this->intensity = intensity;
	this->color = color;
}

Vec3 Light::get_position()
{
	return position;
}

Color Light::get_color()
{
	return color;
}

bool Light::intersected(std::shared_ptr<Ray> ray, std::shared_ptr<Color> pixel, double radius_squared)
{
	Vec3 length = ray->get_origin() - position;

	double a = 1;
	double b = 2 * ray->get_direction().dot_product(length);
	double c = length.dot_product(length) - radius_squared;

	double t0 = (-b - std::sqrt(pow(b, 2) - 4 * a * c)) / (2 * a);
	double t1 = (-b + std::sqrt(pow(b, 2) - 4 * a * c)) / (2 * a);

	if (t0 > ray->get_tnear() && t0 < ray->get_tmax())
	{
		ray->set_tmax(t0);
		return true;
	}
	else if (t1 > ray->get_tnear() && t1 < ray->get_tmax())
	{
		ray->set_tmax(t1);
		return true;
	}

	return false;

}

double Light::get_intensity()
{
	return intensity;
}

Vec3 Light::get_direction(Vec3 P)
{
	return (position - P);
}
