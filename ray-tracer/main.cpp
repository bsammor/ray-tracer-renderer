#define _CRT_SECURE_NO_DEPRECATE
#include "plane.h"
#include "sphere.h"
#include <fstream>
#include "camera.h"
#include "vec3.h"
#include <stdio.h>
#include <iostream>
#include "triangle.h"
#include "ppm.h"
#include <stdlib.h>
#include<stdio.h>
#include <algorithm>
#include <vector>
#include "triangle_mesh.h"
#include <cstdio> 
#include <cstdlib> 
#include "tiny_obj_loader.h"
#include <filesystem>
#include <map> 
namespace fs = std::filesystem;
#define PI 3.14159265358979323846
#define BIAS 0.0001; 
std::map<std::string, ppm> images;

enum ray_type
{
	shadow, camera
};

void save_image(double width, double height, Color* image) 
{
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < width * height; ++i) 
	{
		ofs << (unsigned char)(std::min(double(1), image[i].r) * 255) <<
			(unsigned char)(std::min(double(1), image[i].g) * 255) <<
			(unsigned char)(std::min(double(1), image[i].b) * 255);
	}
	ofs.close();
}

double clamp(const double& lo, const double& hi, const double& v)
{
	return std::max(lo, std::min(hi, v));
}

Ray* create_reflection_ray(Ray *camera_ray, Object* obj) 
{
	Vec3 point = camera_ray->get_intersection_point();
	Vec3 incidence = camera_ray->get_direction();
	Vec3 normal = obj->get_normal(point);
	Vec3 direction = incidence - (normal * 2 * incidence.dot_product(normal));
	return new Ray(point, direction.normalize(), MINIMUM, INFINITY);
}

Vec3 refract(Vec3 I, Vec3 N, double ior)
{
	double cosi = clamp(-1, 1, I.dot_product(N));
	double etai = 1, etat = ior;
	Vec3 n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = (N * -1); }
	double eta = etai / etat;
	double k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? Vec3() : I * eta  + n * (eta * cosi - sqrt(k));
}

double fresnel(Vec3& I, Vec3& N, double ior)
{
	double kr = 0;
	double cosi = clamp(-1, 1, I.dot_product(N));
	double etai = 1, etat = ior;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	double sint = etai / etat * sqrtf(std::max(0.0, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) { kr = 1;}
	else 
	{
		double cost = sqrtf(std::max(0.0, 1 - sint * sint));
		cosi = fabsf(cosi);
		double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;*/
	return kr;
}

bool trace(Ray* ray, std::vector<Object*> scene, ray_type type)
{
	double t = 0.0;
	double u = 0.0;
	double v = 0.0;

	bool intersected = false;
	for (int i = 0; i < scene.size(); i++) 
	{
		if (scene[i]->intersected(ray, i, u, v, t))
		{
			if (type == shadow && scene[i]->get_material() == reflective_refractive) continue;
			ray->u = u;
			ray->v = v;
			ray->set_tmax(t);
			ray->set_index(i);
			intersected = true;
		}
	}

	return intersected;
}



Color cast_ray(Ray* ray, std::vector<Object*> scene, std::vector<Light> lights, int depth = 0) 
{
	if (depth < 5) 
	{
		if (trace(ray, scene, camera))
		{
			int obj_index = ray->get_index();
			Color color;
			Vec3 point = ray->get_intersection_point();
			Vec3 normal = scene[obj_index]->get_normal(point);
			double shadow_bias = 1e-8;

			//testing meshes
			if (TriangleMesh* mesh = dynamic_cast<TriangleMesh*>(scene[obj_index])) {
				
				ppm tex_image;

				for (auto x : images) {
					if (mesh->tex_name == x.first) {
						tex_image = x.second;
						//std::cout << mesh->tex_name << " " << x.first << std::endl;
						//std::cin.get();
					}
				}

				normal = mesh->tri_fnormal;
				Vec3 tex = mesh->tri_tex_coordinates;

				int i = round(tex.x * tex_image.width);
				int j = round(tex.y * tex_image.height);

				Color texture = Color(tex_image.r[i + j * tex_image.width] / 255., tex_image.g[i + j * tex_image.width] / 255., tex_image.b[i + j * tex_image.width] / 255.);
				//ambient light
				//color += texture * 0.2;

				/*for (Light light : lights)
				{
					Vec3 light_dir = light.get_direction(point);
					double r2 = light_dir.dot_product(light_dir);
					double distance = sqrt(r2);
					light_dir = light_dir.normalize();

					color += texture * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir));
				}*/
				return texture;
			}
			else if (scene[obj_index]->get_material() == diffuse)
			{
				//ambient light
				color += scene[obj_index]->get_color() * 0.2;

				for (Light light : lights) 
				{
					Vec3 light_dir = light.get_direction(point);
					double r2 = light_dir.dot_product(light_dir);
					double distance = sqrt(r2);
					light_dir = light_dir.normalize();


					Ray* shadow_ray = new Ray(point + normal * shadow_bias, light_dir, MINIMUM, distance);
					bool covered = !trace(shadow_ray, scene, shadow);

					color += (scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir))) * covered;
				}
				return color;
			}
			else if (scene[obj_index]->get_material() == reflective)
			{
				Ray* reflection_ray = create_reflection_ray(ray, scene[obj_index]);
				color += cast_ray(reflection_ray, scene, lights, depth + 1);

				if (color.r < 0.1 && color.g < 0.1 && color.b < 0.1) {
					color = Color(0.03, 0.77, 0.85);
				}
				return color;
			}
			else if (scene[obj_index]->get_material() == reflective_refractive)
			{
				Color refraction_color, reflection_color;
				// compute fresnel
				double kr;
				Vec3 dir = ray->get_direction();

				kr = fresnel(dir, normal, scene[obj_index]->get_ior());
				bool outside = dir.dot_product(normal) < 0;
				Vec3 bias = normal * BIAS;
				// compute refraction if it is not a case of total internal reflection
				if (kr < 1)
				{
					Vec3 refraction_direction = refract(dir, normal, scene[obj_index]->get_ior()).normalize();
					Vec3 refrection_origin = outside ? point - bias : point + bias;

					Ray* refraction_ray = new Ray(refrection_origin, refraction_direction, MINIMUM, INFINITY);
					refraction_color = cast_ray(refraction_ray, scene, lights, depth + 1);
				}

				Ray* reflection_ray = create_reflection_ray(ray, scene[obj_index]);
				Vec3 reflection_direction = reflection_ray->get_direction();
				Vec3 reflection_origin = outside ? point + bias : point - bias;
				Ray* test = new Ray(reflection_origin, reflection_direction, MINIMUM, INFINITY);
				reflection_color = cast_ray(test, scene, lights, depth + 1);

				// mix the two
				color += reflection_color * kr + refraction_color * (1 - kr);
				return color;
			}
			else if (scene[obj_index]->get_material() == phong)
			{
				Color diffuse, specular;

				for (Light light : lights)
				{
					Vec3 light_dir = light.get_direction(point);
					double r2 = light_dir.dot_product(light_dir);
					double distance = sqrt(r2);
					light_dir = light_dir.normalize();


					Ray* shadow_ray = new Ray(point + normal * shadow_bias, light_dir, MINIMUM, distance);
					bool covered = !trace(shadow_ray, scene, shadow);

					diffuse += (scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir))) * covered;

					Vec3 I = light_dir * -1;
					Vec3 R = I - normal * 2  * I.dot_product(normal);

					 specular += light.get_color() * light.get_intensity() / (4 * PI * r2) * std::pow(std::max(0.0, R.dot_product(ray->get_direction() * -1)), 10) * covered;
				}

				color = diffuse * 0.5 + specular * 0.5;
				return color;
			}
		}
	}
	return Color();
}

int main() 
{
	int width = 200, height = 150;
	Color* image = new Color[width * height];

	std::vector<Object*> scene;
	std::vector<Light> lights;

	std::string path = "textures";
	for (const auto& entry : fs::directory_iterator(path)) {
		auto name = entry.path();
		std::string image = name.string();
		ppm test(2048, 2048);
		test.read(image);
		images.insert(std::pair<std::string, ppm>(image.erase(0, 7), test));
	}

	Camera camera(Vec3(5.0, 10, -50.0), Vec3(0.0, 0.0, 0.0), Vec3(5.0, 11, -50.0), ((50 * 0.5) * PI / 180.0), (double)width/(double)height);
	Light light(Vec3(0.0, 3.0, 0.0), Color(1.0, 1.0, 1.0), 100);
	Light light1(Vec3(-3.0, 3.0, 0.0), Color(1.0, 1.0, 1.0), 100);
	Light light2(Vec3(3.0, 3.0, 0.0), Color(1.0, 1.0, 1.0), 100);
	Light light3(Vec3(0.0, 3.0, -3.0), Color(1.0, 1.0, 1.0), 100);
	Light light4(Vec3(0.0, 3.0, 3.0), Color(1.0, 1.0, 1.0), 100);
	Light light5(Vec3(-3.0, 3.0, -3.0), Color(1.0, 1.0, 1.0), 100);
	Light light6(Vec3(3.0, 3.0, 3.0), Color(1.0, 1.0, 1.0), 100);

	Plane plane(Vec3(0.0, -3.0, 0.0), Vec3(0.0, -2.0, 0.0), Color(0.03, 0.77, 0.85), diffuse);
	Plane plane1(Vec3(0.0, 0.0, -3.0), Vec3(0.0, 0.0, -2.0), Color(1.0, 0.0, 0.0), diffuse);
	Plane plane2(Vec3(3.0, 0.0, 0.0), Vec3(2.0, 0.0, 0.0), Color(0.0, 1.0, 0.0), diffuse);
	Plane plane3(Vec3(-3.0, 0.0, 0.0), Vec3(-2.0, 0.0, 0.0), Color(0.0, 0.0, 1.0), diffuse);
	Sphere sphere(Vec3(3.0, 1.0, -3.0), 1, Color(1.0, 0.0, 0.0), phong);
	sphere.set_ior(1.8);
	Sphere sphere1(Vec3(0.0, 1.0, 0.0), 1, Color(0.0, 1.0, 0.0), diffuse);
	Sphere sphere2(Vec3(-3.0, 1.0, -3.0), 1, Color(1.0, 0.0, 0.0), diffuse);

	//scene.push_back(&plane);
	//scene.push_back(&plane1);
	//scene.push_back(&plane2);
	//scene.push_back(&plane3);
	//scene.push_back(&sphere);
	//scene.push_back(&sphere1);
	//scene.push_back(&sphere2);


	lights.push_back(light);
	//lights.push_back(light1);
	//lights.push_back(light2);
	//lights.push_back(light3);
	//lights.push_back(light4);
	//lights.push_back(light5);
	//lights.push_back(light6);


	TriangleMesh* mesh = new TriangleMesh("cottage.obj", Color(1.0, 0.0, 0.0), diffuse);
	scene.push_back(mesh);

	for (int i = 0; i < width; i++) 
	{
		std::cout << i << std::endl;
		for (int j = 0; j < height; j++) 
		{
			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;
			Color* pixel = image + (i + j * width);

			Ray* camera_ray = camera.create_camera_ray(new_x, new_y);
			*pixel = cast_ray(camera_ray, scene, lights);
		}
	}


	save_image(width, height, image);

	return 0;
}