#ifndef BOX_H
#define BOX_H
#include "ray.h"

class Box
{
public:
	Box();
	Box(Vec3 min, Vec3 max);
	~Box() {}
	bool intersected(Ray* ray);

    Vec3 min, max;  
};

#endif