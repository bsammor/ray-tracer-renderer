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
	Vec3* image = new Vec3[width * height];

	Camera camera(Vec3(0.0, 1.0, -1.0), Vec3(0.0, 1.0, 0.0), Vec3(0.0, 1.0, 0.0), (50 * PI / 180.0), (float)width/(float)height);
	Plane plane(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0));
	Sphere sphere(Vec3(0.0, 1.0, 3.0), 1.0, Vec3(255.0, 0.0, 0.0));

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;
			bool done = false;

			Ray *ray = camera.create_camera_ray(new_x, new_y);
			Vec3* pixel = image + (i + j * width);

			if (plane.intersected(*ray)) {;
				*pixel = Vec3(0.0, 0.0, 255.0);
				done = true;
			}
			if (sphere.intersected(*ray)) {
				*pixel = sphere.get_color();
				done = true;
			}
			/*if (sphere1.intersected(*ray)) {
				*pixel = sphere1.get_color();
				done = true;
			}
			if (sphere2.intersected(*ray)) {
				*pixel = sphere2.get_color();
				done = true;
			}*/
			if (done == false) {
				*pixel = Vec3(255.0, 255.0, 255.0);
			}
		}
	}   
	
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < width * height ; ++i) {
		ofs << (unsigned char)(std::min(double(1), image[i].get_x()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_y()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_z()) * 255);
	}
	ofs.close();

	return 0;
}