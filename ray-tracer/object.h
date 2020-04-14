#pragma once
#include "ray.h"
#include "light.h"
#include "color.h"

enum Material { reflective, diffuse, refractive, phong };

class Object
{
protected:
	Vec3 origin;
	Color color;
	Material material;
	double ior = 0;

public:
	virtual ~Object() {}
	virtual bool intersected(Ray *ray, int index) = 0;
	virtual Ray* create_shadow_ray(Ray *camera_ray, Light light) = 0;
	virtual Vec3 get_normal(Vec3 point) = 0;
	Color get_color() {
		return color;
	}
	Vec3 get_origin() {
		return origin;
	}
	Material get_material() {
		return material;
	}
	void set_ior(double x)
	{
		ior = x;
	}
	double get_ior() {
		return ior;
	}
};

