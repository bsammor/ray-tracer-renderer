#include "camera.h"

camera::camera()
{
	position = vec3();
	direction = vec3();
}

camera::camera(vec3 pos, vec3 dir)
{
	position = pos;
	direction = dir;
}
