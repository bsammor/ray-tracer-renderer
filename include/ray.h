#ifndef RAY_H
#define RAY_H
#include <vec3.h>
#include <color.h>
#include <globals.h>
#include <string>

enum ray_type
{
	primary, secondary
};

class Ray
{
private:
	Vec3 origin, direction;

public:
	double tnear, tmax;
	int obj_index = -1;
	Vec3 fn;
	double u = 0.0, v = 0.0;
	Vec3 vt0, vt1, vt2;
	Color hit_color;
	std::string tex;
	unsigned char a;

	Ray();
	Ray(Vec3 origin, Vec3 direction, double tnear, double tmax);
	inline Vec3 get_origin() {return origin;}
	inline void set_origin(Vec3 origin) {this->origin = origin;}
	inline Vec3 get_direction() {return direction;}
	void set_direction(Vec3 direction) {this->direction = direction;}
	Vec3 get_intersection_point();
};

#endif



