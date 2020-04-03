#include "camera.h"

camera::camera()
{
	origin = vec3(), forward = vec3(), right = vec3(), up = vec3();
	world_height = 0.0, world_width = 0.0;
}

camera::camera(vec3 origin, vec3 target, vec3 up_guide, double fov, double ratio)
{
	this->origin = origin;
	forward = vec3::subtract(target, origin).normalize();
	right = vec3::cross_product(forward, up_guide).normalize();
	up = vec3::cross_product(forward, right);

	world_height = tan(fov);
	world_width = ratio * world_height;
}

ray camera::create_camera_ray(double x, double y)
{
	vec3 result1 = vec3::scalar_mul(vec3::scalar_mul(right, world_width), x);
	vec3 result2 = vec3::scalar_mul(vec3::scalar_mul(up, world_height), y);
	vec3 ray_dir = vec3::add(vec3::add(forward, result1), result2);
	return ray(origin, ray_dir.normalize());
}
