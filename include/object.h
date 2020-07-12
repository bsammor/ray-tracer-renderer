#ifndef OBJECT_H
#define OBJECT_H
#include <light.h>
#include <bbox.h>

enum Material { reflective, diffuse, reflective_refractive, phong };

class Object
{
protected:
	BBOX bbox;
	Color color;
	Material material;
	double ior = 1.1;
	
public:
	virtual bool intersected(std::shared_ptr<Ray> ray, int index) = 0;
	virtual Vec3 get_normal(Vec3 point) = 0;
	virtual BBOX get_bbox() = 0;
	Color get_color() {return color;}
	inline Material get_material() {return material;}
	void set_ior(double x) {ior = x;}
	double get_ior() {return ior;}
};

#endif

