#pragma once
#include "ray.h"
#include "light.h"

class Object
{
protected:
	Vec3 color, origin, albedo;
public:
	virtual ~Object() {}
	virtual bool intersected(Ray *ray) = 0;
	virtual Ray* create_shadow_ray(Vec3 point, Light light) = 0;
	Vec3 get_color() {
		return color;
	}
	Vec3 get_origin() {
		return origin;
	}
	Vec3 get_albedo() {
		return albedo;
	}
};

