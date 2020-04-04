#include "camera.h"
#include <iostream>
using namespace std;

Camera::Camera()
{
	origin = Vec3(), forward = Vec3(), right = Vec3(), up = Vec3();
	world_height = 0.0, world_width = 0.0;
}

Camera::Camera(Vec3 origin, Vec3 target, Vec3 up_guide, double fov, double ratio)
{
	this->origin = origin;
	forward = Vec3::subtract(target, origin).normalize();
	right = Vec3::cross_product(forward, up_guide).normalize();
	up = Vec3::cross_product(right, forward);
	
	world_height = tan(fov);
	world_width = ratio * world_height;
}

Ray* Camera::create_camera_ray(double x, double y)
{
	Vec3 result1 = Vec3::scalar_mul(Vec3::scalar_mul(right, world_width), x);
	Vec3 result2 = Vec3::scalar_mul(Vec3::scalar_mul(up, world_height), y);
	Vec3 ray_dir = Vec3::add(Vec3::add(forward, result1), result2);
	Ray *lol = new Ray(this->origin, ray_dir.normalize());
	return lol;
}
