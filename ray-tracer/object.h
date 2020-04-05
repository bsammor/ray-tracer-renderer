#pragma once
#include "ray.h"

class Object
{
protected:
	Vec3 color, origin;
public:
	virtual ~Object() {}
	virtual bool intersected(Ray ray) = 0;
	Vec3 get_color() {
		return color;
	}
	Vec3 get_origin() {
		return origin;
	}
};

