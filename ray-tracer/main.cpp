#define _CRT_SECURE_NO_DEPRECATE
#include "plane.h"
#include "sphere.h"
#include <fstream>
#include "camera.h"
#include "vec3.h"
#include <stdio.h>
#include <iostream>
#include "triangle.h"
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include "triangle_mesh.h"
#include <cstdio> 
#include <cstdlib> 
#include "tiny_obj_loader.h"
#define PI 3.14159265358979323846

void save_image(double width, double height, Color* image) 
{
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(double(1), image[i].r) * 255) <<
			(unsigned char)(std::min(double(1), image[i].g) * 255) <<
			(unsigned char)(std::min(double(1), image[i].b) * 255);
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

double clamp(const double& lo, const double& hi, const double& v)
{
	return std::max(lo, std::min(hi, v));
}

Vec3 refract(Vec3 I, Vec3 N, double ior)
{
	double cosi = clamp(-1, 1, I.dot_product(N));
	double etai = 1, etat = ior;
	Vec3 n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = N * -1; }
	double eta = etai / etat;
	double k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? Vec3() : n * I * (eta + (eta * cosi - sqrt(k)));
}

double fresnel(Vec3 I, Vec3 N, double ior) {
	double cosi = clamp(-1, 1, I.dot_product(N));
	double etai = 1, etat = ior;
	double kr = 0.0;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	double sint = etai / etat * sqrt(std::max(0.0, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		kr = 1;
	}
	else {
		double cost = sqrt(std::max(0.0, 1 - sint * sint));
		cosi = fabsf(cosi);
		double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	return kr;
}

Color trace(Ray* camera_ray, std::vector<Light> lights, std::vector<Object*> scene, int depth = 0) 
{
	int obj_index = camera_ray->get_index();
	Color color;
	
	//ambient light
	color += scene[obj_index]->get_color() * 0.2;

	if (scene[obj_index]->get_material() == diffuse)
	{
		Vec3 point = camera_ray->get_intersection_point();
		Vec3 normal = scene[obj_index]->get_normal(point);
		double shadow_bias = 1e-4;

		for (Light light : lights) {
			Vec3 light_dir = light.get_direction(point);
			double r2 = light_dir.dot_product(light_dir);
			double distance = sqrt(r2);
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
				color += (scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir)));
			}
			else {
				color += Color();
			}
		}
	}
	else if (scene[obj_index]->get_material() == reflective) {
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
	else if (scene[obj_index]->get_material() == refractive) {
		if (depth < 5) {
			for (Light light : lights) {
				Color refraction_color = Color();
				Color reflection_color = Color();

				Vec3 dir = camera_ray->get_direction();
				Vec3 normal = scene[obj_index]->get_normal(camera_ray->get_intersection_point());
				Vec3 point = camera_ray->get_intersection_point();
				double ior = scene[obj_index]->get_ior();
				double kr = fresnel(dir, normal, ior);

				bool outside = dir.dot_product(normal) < 0;
				Vec3 bias = normal * 1e-4;

				if (kr < 1) {
					Vec3 refractionDirection = refract(dir, normal, ior).normalize();
					Vec3 refractionRayOrig = outside ? point - bias : point + bias;
					Ray* refraction_ray = new Ray(refractionRayOrig, refractionDirection, MINIMUM, INFINITY);
					for (int i = 0; i < scene.size(); i++) {
						if (i != obj_index)
							scene[i]->intersected(refraction_ray, i);
					}
					if (refraction_ray->get_index() != -1) {
						refraction_color = trace(refraction_ray, lights, scene, depth + 1);
					}
				}

				Ray* reflection_ray = create_reflection_ray(camera_ray, light, scene[obj_index]);
				for (int i = 0; i < scene.size(); i++) {
					if (i != obj_index)
						scene[i]->intersected(reflection_ray, i);
				}
				if (reflection_ray->get_index() != -1) {
					reflection_color = trace(reflection_ray, lights, scene, depth + 1);
				}
				// mix the two
				color += ( reflection_color* kr + refraction_color * (1 - kr));
			}
		}

	}
	else if (scene[obj_index]->get_material() == phong) {
		Vec3 point = camera_ray->get_intersection_point();
		Vec3 normal = scene[obj_index]->get_normal(point);
		Color diffuse;
		Color specular;
		double shadow_bias = 1e-4;

		for (Light light : lights) {
			Vec3 light_dir = light.get_direction(point);
			double r2 = light_dir.dot_product(light_dir);
			double distance = sqrt(r2);
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
				diffuse = diffuse + (scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir)));
			}
			else {
				diffuse = diffuse * 0.96;
			}
		}

		for (Light light : lights) {
			Vec3 lightDir = light.get_position() - camera_ray->get_intersection_point();
			Vec3 V = camera_ray->get_direction() * -1;
			// Blinn-Phong
			Vec3 H = (lightDir + V).normalize();
			double NdotH = normal.dot_product(H);

			double phong = std::pow(NdotH, 1250);
			specular =
				light.get_color() * std::fmax(0, phong) *
				light.get_intensity(); // sceneObject->GetSpecular();
			// add
			// or
			// no?
			specular = specular * 0.04;
		}
		color += diffuse;
		color += specular;
	}

	return color;
}

int main() 
{
	int width = 640, height = 480;
	Color* image = new Color[width * height];
	std::vector<Object*> scene;
	std::vector<Light> lights;
	std::vector<TriangleMesh*> meshes;

	Vec3 v0(-1, -1, 10);
	Vec3 v1(1, -1, 10);
	Vec3 v2(0, 1, 10);

	Camera camera(Vec3(0.0, 10.0, 10), Vec3(0.0, 0.0, 0.0), Vec3(0.0, 11.0, 10), ((50 * 0.5) * PI / 180.0), (double)width/(double)height);
	Light light(Vec3(0.0, 1.0, 2.0), Color(1.0, 1.0, 1.0), 100);
	Light light1(Vec3(7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);
	Light light2(Vec3(-7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);

	Plane plane(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Color(0.03, 0.77, 0.85), diffuse);
	Sphere sphere(Vec3(0.0, 1.0, 0.0), 1, Color(1.0, 0.0, 0.0), phong);
	Sphere sphere1(Vec3(-3.0, 1.0, -2.0), 1, Color(1.0, 1.0, 1.0), diffuse);
	Sphere sphere2(Vec3(3.0, 1.0, -3.0), 1, Color(1.0, 0.5, 0.0), diffuse);
	Triangle triangle(v1, v0, v2, Color(1.0, 1.0, 1.0), diffuse);
	scene.push_back(&plane);
	//scene.push_back(&triangle);
	//scene.push_back(&sphere1);
	//scene.push_back(&sphere2);
	scene.push_back(&sphere);
	lights.push_back(light);
	//lights.push_back(light2);
	//lights.push_back(light1);

	//TriangleMesh* mesh = new TriangleMesh("obj/redcat.obj");
	//scene.push_back(mesh);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;

			Ray* camera_ray = camera.create_camera_ray(new_x, new_y);
			Color* pixel = image + (i + j * width);

			for (int i = 0; i < scene.size(); i++)
				scene[i]->intersected(camera_ray, i);

			if (camera_ray->get_index() != -1) {
				/*if (dynamic_cast<Plane*>(scene[camera_ray->get_index()])) {
					if (camera_ray->get_intersection_point().z > -10) {
						unsigned square = int(floor(camera_ray->get_intersection_point().x)) +
							int(floor(camera_ray->get_intersection_point().z));  // (floor() rounds down)
						if (square % 2 == 0)                           // black tile
							*pixel = Color();
						else  // white tile
							*pixel = Color(1, 1, 1);
					}
					else
						*pixel = Color(0, 0, 0);
					
				}*/
				//else {
					//std::cout << "yes";
					//*pixel = Color(camera_ray->u, camera_ray->v, (1 - camera_ray->u - camera_ray->v));
					*pixel = trace(camera_ray, lights, scene);
				//}
			}
			else
				*pixel = Color(0.0, 0.0, 0.0);
		}
	}


	save_image(width, height, image);

	return 0;
}