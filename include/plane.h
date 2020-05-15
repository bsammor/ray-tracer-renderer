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
	virtual ~Plane() override {}
	virtual bool intersected(std::shared_ptr<Ray> ray, int index, double& u, double& v, double& tmax);
	Vec3 get_normal(Vec3 point);
	virtual BBOX get_bbox();
};

#endif


