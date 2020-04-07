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
#include "object.h"
#define PI 3.14159265358979323846
using namespace std;


void save_image(double width, double height, Vec3* image) {
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(double(1), image[i].get_x()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_y()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_z()) * 255);
	}
	ofs.close();
}

int main() {
	int width = 1920, height = 1080;
	Vec3* image = new Vec3[width * height];
	vector<Object*> scene;

	Camera camera(Vec3(0.0, 3.0, 5.0), Vec3(0.0, 1.0, -1.0), Vec3(0.0, 1.0, 0.0), (50 * PI / 180.0), (float)width/(float)height);
	Light light(Vec3(0.0, 3.0, 5.0), Vec3(1.0, 1.0, 1.0), 10, 1);

	Plane plane(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.18, 0.18, 0.18));
	Sphere sphere(Vec3(0.0, 1.0, 2.0), 1, Vec3(0.0, 0.0, 0.0), Vec3(0.5, 0.0, 1.0));
	scene.push_back(&plane);
	scene.push_back(&sphere);



	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;

			Ray *camera_ray = camera.create_camera_ray(new_x, new_y);
			Vec3* pixel = image + (i + j * width);

			for (int i = 0; i < scene.size(); i++) {
				if (scene[i]->intersected(camera_ray)) {
					Vec3 N = (camera_ray->get_intersection_point() - scene[i]->get_origin()).normalize();
					Vec3 L = light.get_direction(camera_ray->get_intersection_point());
					double dist = L.magnitude();
					L = L.normalize();
					double new_x = (scene[i]->get_albedo() / PI).get_x() * light.get_color().get_x();
					double new_y = (scene[i]->get_albedo() / PI).get_y() * light.get_color().get_y();
					double new_z = (scene[i]->get_albedo() / PI).get_z() * light.get_color().get_z();
					Vec3 color = Vec3(new_x, new_y, new_z) * light.get_intensity() * std::max(0.0, N.dot_product(L));
					//cout << color.get_x() << " " << color.get_y() << " " << color.get_z() << endl;
					*pixel = color;
				}
				else
					*pixel = Vec3(0.5, 0.5, 0.5);
			}
		}
	}   

	save_image(width, height, image);

	return 0;
}