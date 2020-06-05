#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "object.h"
#include <cstdint>
#include <string>

extern uint64_t numRayTrianglesTests;
extern uint64_t numRayTrianglesIsect;

class Triangle : public Object
{
public:
	Triangle();
	Triangle(Vec3 v0, Vec3 v1, Vec3 v2, Color color, Material material);
	bool intersected(std::shared_ptr<Ray> ray, int index, double& u, double& v, double& tmax) override;
	bool intersectedP(std::shared_ptr<Ray> ray) const override;
	Vec3 get_normal(Vec3 point) override;
	BBOX get_bbox() override;
	Vec3 v0, v1, v2, fn, vt0, vt1, vt2;
	std::string tex;
};

#endif