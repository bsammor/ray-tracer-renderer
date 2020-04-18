#pragma once
#include "object.h"

class Plane : public Object
{
private:
	Vec3 normal;
	Vec3 position;
public:
	Plane();
	Plane(Vec3 position, Vec3 normal, Color color, Material material);
	virtual ~Plane() override {}
	virtual bool intersected(Ray* ray, int index);
	Vec3 get_normal(Vec3 point);
};




