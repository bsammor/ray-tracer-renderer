#pragma once
#include "ray.h"
#include "light.h"
#include "color.h"

class Object
{
protected:
	Vec3 origin;
	Color color;
public:
	virtual ~Object() {}
	virtual bool intersected(Ray *ray) = 0;
	virtual Ray* create_shadow_ray(Ray *camera_ray, Light light) = 0;
	Color get_color() {
		return color;
	}
	Vec3 get_origin() {
		return origin;
	}
};

