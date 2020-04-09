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


void save_image(double width, double height, Color* image) 
{
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(double(1), image[i].get_r()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_g()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_b()) * 255);
	}
	ofs.close();
}

Ray* create_reflection_ray(Ray *camera_ray, Light light, Object* obj) 
{
	Vec3 point = camera_ray->get_intersection_point();
	Vec3 incidence = camera_ray->get_direction();
	Vec3 normal = obj->get_normal(point);
	Vec3 direction = incidence - (normal * 2 * incidence.dot_product(normal));
	return new Ray(point, direction.normalize(), MINIMUM, INFINITY);
}

Color trace(Ray* camera_ray, std::vector<Light> lights, std::vector<Object*> scene, int depth = 0) 
{
	int obj_index = camera_ray->get_index();
	Color color;
	if (scene[obj_index]->get_material() == diffuse)
	{
		Vec3 point = camera_ray->get_intersection_point();
		Vec3 normal = scene[obj_index]->get_normal(point);
		double shadow_bias = 1e-4;

		for (Light light : lights) {
			Vec3 light_dir = light.get_direction(point);
			double r2 = light_dir.dot_product(light_dir);
			double distance = sqrtl(r2);
			light_dir = light_dir.normalize();

			Ray* shadow_ray = new Ray(point + normal * shadow_bias, light_dir, MINIMUM, distance);
			bool covered = false;
			for (int j = 0; j < scene.size(); j++) {
				if (j != obj_index && scene[j]->intersected(shadow_ray, j)) {
					covered = true;
					break;
				}
			}
			if (!covered) {
				color = color + (scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir)));
			}
			else {
				color = color + Color();
			}
		}
	}
	else if (scene[obj_index]->get_material() == specular) {
		if (depth < 5) {
			for (Light light : lights) {
				Ray* reflection_ray = create_reflection_ray(camera_ray, light, scene[obj_index]);

				for (int i = 0; i < scene.size(); i++) {
					if (i != obj_index)
						scene[i]->intersected(reflection_ray, i);
				}
				if (reflection_ray->get_index() != -1) {
					color = trace(reflection_ray, lights, scene, depth + 1);
				}
				else {
					color = Color(0.0, 0.0, 0.0);
				}
			}
		}
	}
	return color;
}

int main() 
{
	int width = 1920, height = 1080;
	Color* image = new Color[width * height];
	std::vector<Object*> scene;
	std::vector<Light> lights;

	Camera camera(Vec3(0.0, 5.0, 15), Vec3(0.0, 0.0, -1.0), Vec3(0.0, 1.0, .0), (20 * PI / 180.0), (float)width/(float)height);
	Light light(Vec3(0.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);
	Light light1(Vec3(7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);
	Light light2(Vec3(-7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);

	Plane plane(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Color(0.03, 0.77, 0.85), diffuse);
	Sphere sphere(Vec3(0.0, 1.0, 0.0), 1, Color(1.0, 0.4, 0.7), specular);
	Sphere sphere1(Vec3(-3.0, 1.0, 0.0), 1, Color(1.0, 1.0, 1.0), diffuse);
	Sphere sphere2(Vec3(3.0, 1.0, 0.0), 1, Color(1.0, 0.5, 0.0), diffuse);
	scene.push_back(&plane);
	scene.push_back(&sphere1);
	scene.push_back(&sphere2);
	scene.push_back(&sphere);
	lights.push_back(light);
	//lights.push_back(light2);
	//lights.push_back(light1);
	

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;

			Ray *camera_ray = camera.create_camera_ray(new_x, new_y);
			Color *pixel = image + (i + j * width);

			for (int i = 0; i < scene.size(); i++)
				scene[i]->intersected(camera_ray, i);

			if (camera_ray->get_index() != -1) {
				*pixel = trace(camera_ray, lights, scene);
			}
			else
				*pixel = Color(0.0, 0.0, 0.0);
		}
	}   

	save_image(width, height, image);

	return 0;
}