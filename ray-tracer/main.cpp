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
	int width = 640	, height = 480;
	Color* image = new Color[width * height];
	std::vector<Object*> scene;

	Camera camera(Vec3(0.0, 0.0, 10.0), Vec3(0.0, 0.0, -1.0), Vec3(0.0, 1.0, 0.0), (20 * PI / 180.0), (float)width/(float)height);
	Light light(Vec3(0.0, 0.0, 5.0), Color(1.0, 1.0, 1.0), 800);

	Plane plane(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Color(0.3, 0.3, 0.3));
	Sphere sphere(Vec3(0.0, 0.0, 0.0), 1, Color(1.0, 0.4, 0.7));
	Sphere sphere1(Vec3(3.0, 1.0, 0.0), 1, Color(0.004, 0.47, 0.114));
	Sphere sphere2(Vec3(-3.0, 1.0, 0.0), 1, Color(1.0, 0.5, 0.0));
	//scene.push_back(&plane);
	//scene.push_back(&sphere1);
	//scene.push_back(&sphere2);
	scene.push_back(&sphere);




	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;

			Ray *camera_ray = camera.create_camera_ray(new_x, new_y);
			Color* pixel = image + (i + j * width);
			*pixel = Color(0.0, 0.0, 0.0);

			for (int i = 0; i < scene.size(); i++) {
				if (scene[i]->intersected(camera_ray)) {
					if (dynamic_cast<Sphere*>(scene[i])) {

						Color* light_intensity = Color(0.0, 0.0, 0.0);
						Ray* shadow_ray = sphere.create_shadow_ray(camera_ray, light, light_intensity);

						Vec3 light_dir = light.get_origin() - camera_ray->get_intersection_point();
						double radius_squared = light_dir.dot_product(light_dir);

						if (light.intersected(shadow_ray, pixel, radius_squared)) {
							Vec3 normal = (camera_ray->get_intersection_point() - scene[i]->get_origin()).normalize();
							Vec3 light_dir = light.get_direction(camera_ray->get_intersection_point());
							double r2 = light_dir.dot_product(light_dir);
							light_dir = light_dir.normalize();

							*pixel = scene[i]->get_color() / PI * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir));
						}
					}
					else if (dynamic_cast<Plane*>(scene[i])) {
						*pixel = scene[i]->get_color();
					}
				}
			}
		}
	}   

	save_image(width, height, image);

	return 0;
}