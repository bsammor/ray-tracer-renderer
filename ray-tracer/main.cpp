#define _CRT_SECURE_NO_DEPRECATE
#include "plane.h"
#include "sphere.h"
#include <fstream>
#include "camera.h"
#include "vec3.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#define PI 3.14159265358979323846


void save_image(double width, double height, Color* image) {
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(double(1), image[i].get_r()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_g()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_b()) * 255);
	}
	ofs.close();
}

int main() {
	int width = 640, height = 480;
	Color* image = new Color[width * height];
	std::vector<Object*> scene;
	std::vector<Light> lights;

	Camera camera(Vec3(0.0, 5.0, 10), Vec3(0.0, 0.0, -1.0), Vec3(0.0, 1.0, .0), (20 * PI / 180.0), (float)width/(float)height);
	Light light(Vec3(0.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);
	Light light1(Vec3(7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);
	Light light2(Vec3(-7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);

	Plane plane(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Color(0.03, 0.77, 0.85), diffuse);
	Sphere sphere(Vec3(0.0, 1.0, 0.0), 1, Color(1.0, 0.4, 0.7), specular);
	Sphere sphere1(Vec3(-3.0, 1.0, 0.0), 1, Color(0.004, 0.47, 0.114), diffuse);
	Sphere sphere2(Vec3(3.0, 1.0, 0.0), 1, Color(1.0, 0.5, 0.0), diffuse);
	scene.push_back(&plane);
	//scene.push_back(&sphere1);
	//scene.push_back(&sphere2);
	scene.push_back(&sphere);
	lights.push_back(light);
	//lights.push_back(light1);
	//lights.push_back(light2);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;

			Ray *camera_ray = camera.create_camera_ray(new_x, new_y);
			Color *pixel = image + (i + j * width);
			bool intersected_once = false;

			for (int i = 0; i < scene.size(); i++)
				scene[i]->intersected(camera_ray, i);

			if (camera_ray->get_index() != -1) {
				int obj_index = camera_ray->get_index();
				Vec3 point = camera_ray->get_intersection_point();
				Vec3 normal = scene[obj_index]->get_normal(point);
				double shadow_bias = 1e-4;
				for (Light light : lights) {

					Color color;
					Vec3 light_dir = light.get_direction(point);
					double r2 = light_dir.dot_product(light_dir);
					double distance = sqrtl(r2);
					light_dir = light_dir.normalize();

					Ray* shadow_ray = new Ray(point + normal * shadow_bias, light_dir, MINIMUM, distance);
					bool covered = false;
					for (int j = 0; j < scene.size(); j++) {
						if (j != i && scene[j]->intersected(shadow_ray, j)) {
							covered = true;
							break;
						}
					}
					if (!covered) {
						color = scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir));
					}
					else {
						color = Color();
					}
					*pixel = *pixel + color;
				}
			}
			else
				*pixel = Color(0.0, 0.0, 0.0);
		}
	}   

	save_image(width, height, image);

	return 0;
}