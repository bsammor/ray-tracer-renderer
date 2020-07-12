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


