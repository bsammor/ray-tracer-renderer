#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"
#include <cstdint>

extern uint64_t numRayTrianglesTests;
extern uint64_t numRayTrianglesIsect;
extern uint64_t numPrimaryRays;

class Triangle : public Object
{
public:
	Triangle();
	Triangle(Vec3 v0, Vec3 v1, Vec3 v2, Color color, Material material);
	virtual ~Triangle() {}
	virtual bool intersected(Ray* ray, int index, double& u, double& v, double& tmax);
	virtual Vec3 get_normal(Vec3 point);
	virtual void calculate_bbox();
	Vec3 v0, v1, v2, fn;
};

#endif