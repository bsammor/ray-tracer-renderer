#pragma once
#include "vec3.h"
#include <math.h>
#include "ray.h"

class Camera
{
private:
	Vec3 origin, forward, right, up;
	double world_height, world_width;
public:
	Camera();
	Camera(Vec3 origin, Vec3 target, Vec3 up_guide, double fov, double ratio);
	Ray* create_camera_ray(double x, double y);
};

