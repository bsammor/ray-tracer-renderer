// Theory and Implementation inspired by 
// https://www.youtube.com/watch?v=RIgc5J_ZGu8&list=PLAqGIYgEAxrUO6ODA0pnLkM2UOijerFPv
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes
#ifndef PLANE_H
#define PLANE_H
#include "object.h"

class Plane : public Object
{
private:
	Vec3 normal;
	Vec3 position;
	
public:
	Plane();
	Plane(Vec3 position, Vec3 normal, Color color, Material material);
	bool intersected(std::shared_ptr<Ray> ray, int index) override;
	Vec3 get_normal(Vec3 point) override;
	BBOX get_bbox() override;
};

#endif


