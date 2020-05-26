#ifndef OBJECT_H
#define OBJECT_H
#include "ray.h"
#include "light.h"
#include "color.h"
#include <bbox.h>
#include <memory>

enum Material { reflective, diffuse, reflective_refractive, phong };

class Object
{
public:
	BBOX bbox;
	Color color;
	Material material;
	double ior = 1.1;
	virtual bool intersected(std::shared_ptr<Ray> ray, int index, double& u, double& v, double& t) = 0;
	virtual bool intersectedP(std::shared_ptr<Ray> ray) const = 0;
	virtual Vec3 get_normal(Vec3 point) = 0;
	virtual BBOX get_bbox() = 0;
	Color get_color() 
	{
		return color;
	}
	inline Material get_material() 
	{
		return material;
	}
	void set_ior(double x)
	{
		ior = x;
	}
	double get_ior() 
	{
		return ior;
	}
};

#endif

