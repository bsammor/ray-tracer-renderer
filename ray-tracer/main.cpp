#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include "vec3.h"
#include "light.h"
#include "sphere.h"
#include <vector>
#include "ray.h"
#include "camera.h"
#include <fstream>
#include "Plane.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#define PI 3.14159265358979323846
using namespace std;

int main() {
	int width = 640, height = 480;
	vec3* image = new vec3[width * height];
	camera camera(vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), vec3(), (10 * PI / 180), (float)width/(float)height);
	plane plane;


	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			double xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			double yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			//TODO: try the raytracer.cpp functions here to render a plane or sphere.
			vec3 raydir = vec3::normalize(vec3(xx, yy, -1));
			ray *ray = camera.create_camera_ray(new_x, new_y);
			if (plane.is_intersected(*ray)) {
				image[i * j] = vec3(0.0,255.0,0.0);
			}
			else {
				image[i * j] = vec3();
			}

		}

	
	}   
	
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(double(1), image[i].get_x()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_y()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_z()) * 255);
	}
	ofs.close();

	return 0;
}