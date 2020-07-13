// Theory and Implementation insipred by 
// https://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays
// https://www.youtube.com/watch?v=LRN_ewuN_k4&list=PLAqGIYgEAxrUO6ODA0pnLkM2UOijerFPv&index=6
#ifndef CAMERA_H
#define CAMERA_H
#include <ray.h>
#include <memory>

class Camera
{
private:
	Vec3 position, forward, right, up;
	double world_height, world_width;

public:
	Camera();
	Camera(Vec3 position, Vec3 target, Vec3 up_guide, double fov, double ratio);
	std::shared_ptr<Ray> create_camera_ray(double x, double y);
};

#endif

