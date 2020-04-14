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
#include "TriangleMesh.h"
#include <cstdio> 
#include <cstdlib> 
#include <memory> 
#include <vector> 
#include <utility> 
#include <cstdint> 
#include <iostream> 
#include <fstream> 
#include <cmath> 
#include <sstream> 
#include <chrono> 
#define PI 3.14159265358979323846

TriangleMesh* loadPolyMeshFromFile(const char* file)
{
	std::ifstream ifs;
	try { 
		ifs.open(file);
		if (ifs.fail()) throw;
		std::stringstream ss;
		ss << ifs.rdbuf();
		uint32_t numFaces;
		ss >> numFaces;
		uint32_t* faceIndex(new uint32_t[numFaces]);
		uint32_t vertsIndexArraySize = 0;
		// reading face index array
		for (uint32_t i = 0; i < numFaces; ++i) {
			ss >> faceIndex[i];
			vertsIndexArraySize += faceIndex[i];
		}
		uint32_t* vertsIndex(new uint32_t[vertsIndexArraySize]);
		uint32_t vertsArraySize = 0;
		// reading vertex index array
		for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
			ss >> vertsIndex[i];
			if (vertsIndex[i] > vertsArraySize) vertsArraySize = vertsIndex[i];
		}
		vertsArraySize += 1;
		// reading vertices
		Vec3* verts(new Vec3[vertsArraySize]);
		for (uint32_t i = 0; i < vertsArraySize; ++i) {
			ss >> verts[i].x >> verts[i].y >> verts[i].z;
		}
		// reading normals
		Vec3* normals(new Vec3[vertsIndexArraySize]);
		for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
			ss >> normals[i].x >> normals[i].y >> normals[i].z;
		}
		// reading st coordinates
		Vec3* st(new Vec3[vertsIndexArraySize]);
		for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
			ss >> st[i].x >> st[i].y;
		}
		return new TriangleMesh(numFaces, faceIndex, vertsIndex, verts, normals, st);
	}
	catch (...) {
		ifs.close();
	}
	ifs.close();

	return nullptr;
}

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
	if (true) {
		color = color + scene[obj_index]->get_color() * 0.2;
	}
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
				color = color + (scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir)));
			}
			else {
				color = color + Color();
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
				color = color + ( reflection_color* kr + refraction_color * (1 - kr));
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
			Vec3 lightDir = light.get_origin() - camera_ray->get_intersection_point();
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
		color = color + diffuse;
		color = color + specular;
	}

	return color;
}

int main() 
{
	TriangleMesh* test = loadPolyMeshFromFile("./cow.geo");
	std::cout << test->get_nfaces() << std::endl;
	int width = 640, height = 480;
	Color* image = new Color[width * height];
	std::vector<Object*> scene;
	std::vector<Light> lights;

	Vec3 v0(-1, -1, 10);
	Vec3 v1(1, -1, 10);
	Vec3 v2(0, 1, 10);

	Camera camera(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0), ((51.52 * 0.5) * PI / 180.0), (double)width/(double)height);
	Light light(Vec3(0.0, 1.0, 0.0), Color(1.0, 1.0, 1.0), 100);
	Light light1(Vec3(7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);
	Light light2(Vec3(-7.0, 10.0, 0.0), Color(1.0, 1.0, 1.0), 1000);

	Plane plane(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Color(0.03, 0.77, 0.85), diffuse);
	Sphere sphere(Vec3(0.0, 1.0, -3.0), 1, Color(1.0, 0.0, 0.0), phong);
	Sphere sphere1(Vec3(-3.0, 1.0, -2.0), 1, Color(1.0, 1.0, 1.0), diffuse);
	Sphere sphere2(Vec3(3.0, 1.0, -3.0), 1, Color(1.0, 0.5, 0.0), diffuse);
	Triangle triangle(v1, v0, v2, Color(0.03, 0.77, 0.85), diffuse);
	//scene.push_back(&plane);
	//scene.push_back(&triangle);
	//scene.push_back(&sphere1);
	//scene.push_back(&sphere2);
	//scene.push_back(&sphere);
	//lights.push_back(light);
	//lights.push_back(light2);
	//lights.push_back(light1);
	scene.push_back(test);
	

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (-2.0 * j) / height + 1.0;

			Ray *camera_ray = camera.create_camera_ray(new_x, new_y);
			Color *pixel = image + (i + j * width);

			for (int i = 0; i < scene.size(); i++)
				scene[i]->intersected(camera_ray, i);

			if (camera_ray->get_index() != -1) {
				std::cout << "yes";
				//*pixel = scene[camera_ray->get_index()]->get_color();
				//*pixel = trace(camera_ray, lights, scene);
			} 
			else
				*pixel = Color(0.0, 0.0, 0.0);
		}
	}   

	save_image(width, height, image);

	return 0;
}