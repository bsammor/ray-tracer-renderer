#pragma once
#include "vec3.h"
#include <math.h>
#include "ray.h"

class camera
{
private:
	vec3 origin, forward, right, up;
	double world_height, world_width;
public:
	camera();
	camera(vec3 origin, vec3 target, vec3 up_guide, double fov, double ratio);
	ray* create_camera_ray(double x, double y);
};

