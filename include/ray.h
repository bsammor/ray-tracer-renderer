#ifndef RAY_H
#define RAY_H
#include <vec3.h>
#include <color.h>
#include <string>

enum ray_type
{
	shadow, camera
};

class Ray
{
private:
	double tnear, tmax;
	int obj_index = -1;
public:
	Ray();
	Ray(Vec3 origin, Vec3 direction, double tnear, double tmax);
	inline Vec3 get_origin() {return origin;}
	inline void set_origin(Vec3 origin) {this->origin = origin;}
	void set_tmax(double point);
	inline Vec3 get_direction() {return direction;}
	void set_direction(Vec3 direction) {this->direction = direction;}
	Vec3 get_intersection_point();
	double get_tnear();
	inline double get_tmax() {return tmax;}
	void set_index(int i);
	int get_index();

	int intersections = 0; 
	Vec3 fn;
	double u = 0.0, v = 0.0;
	Vec3 vt0, vt1, vt2;
	Color hitcolor;
	Vec3 origin, direction;
	std::string tex;
	unsigned char a;
};

#endif



