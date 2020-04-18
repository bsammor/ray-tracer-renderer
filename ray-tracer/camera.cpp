#include "camera.h"

Camera::Camera()
{
	position = Vec3(), forward = Vec3(), right = Vec3(), up = Vec3();
	world_height = 0.0, world_width = 0.0;
}

Camera::Camera(Vec3 position, Vec3 look_at, Vec3 up_guide, double fov, double ratio)
{
	this->position = position;
	forward = (look_at - position).normalize();
	right = (forward.cross_product(up_guide)).normalize();
	up = right.cross_product(forward);

	world_height = tan(fov);
	world_width = ratio * world_height;
}

Ray* Camera::create_camera_ray(double x, double y)
{
	Vec3 result1 = right * world_width * x;
	Vec3 result2 = up * world_height * y;
	Vec3 ray_dir = forward + result1 + result2;
	return new Ray(this->position, ray_dir.normalize(), MINIMUM, INFINITY);
}
