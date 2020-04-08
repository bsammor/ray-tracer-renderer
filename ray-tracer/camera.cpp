#include "camera.h"

Camera::Camera()
{
	origin = Vec3(), forward = Vec3(), right = Vec3(), up = Vec3();
	world_height = 0.0, world_width = 0.0;
}

Camera::Camera(Vec3 origin, Vec3 look_at, Vec3 up_guide, double fov, double ratio)
{
	this->origin = origin;
	forward = (look_at - origin).normalize();
	right = ((forward * up_guide).normalize());
	up = right * forward;

	world_height = tan(fov);
	world_width = ratio * world_height;
}

Ray* Camera::create_camera_ray(double x, double y)
{
	Vec3 result1 = right * world_width * x;
	Vec3 result2 = up * world_height * y;
	Vec3 ray_dir = forward + result1 + result2;
	Ray *ray = new Ray(this->origin, ray_dir.normalize(), MINIMUM, INFINITY);
	return ray;
}
