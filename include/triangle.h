// Theory and Implementation insipred by 
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle
#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <object.h>

extern uint64_t num_ray_prim_tests;
extern uint64_t num_ray_prim_isects;

class Triangle : public Object
{
public:
	Triangle();
	Triangle(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 vt0, Vec3 vt1, Vec3 vt2, Color color, Material material);
	bool intersected(std::shared_ptr<Ray> ray, int index) override;
	Vec3 get_normal(Vec3 point) override;
	BBOX get_bbox() override;

	std::string tex;
	Vec3 v0, v1, v2, fn, vt0, vt1, vt2;
};

#endif