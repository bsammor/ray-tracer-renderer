#pragma once
#include "object.h"
class Triangle : public Object
{
private:
	Vec3 v1, v2, v3;
public:
	Triangle();
	Triangle(Vec3 v1, Vec3 v2, Vec3 v3);
	virtual ~Triangle() {}
	virtual bool intersected(Ray ray);

};

