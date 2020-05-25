#ifndef RAY_H
#define RAY_H
#include <vec3.h>

class Ray
{
private:
	Vec3 origin, direction;
	double tnear, tmax;
	int obj_index = -1;
public:
	Ray();
	Ray(Vec3 origin, Vec3 direction, double tnear, double tmax);
	inline Vec3 get_origin() {return origin;}
	void set_tmax(double point);
	inline Vec3 get_direction() {return direction;}
	Vec3 get_intersection_point();
	double get_tnear();
	inline double get_tmax() {return tmax;}
	void set_index(int i);
	int get_index();

	double u = 0.0, v = 0.0;
	int intersections = 0;
	Vec3 invdir; 
    int sign[3]; 
	Vec3 fn;
};

#endif



