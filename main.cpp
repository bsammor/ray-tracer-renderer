#include <string>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <cstdio> 
#include <cstdlib> 
#include <map> 
#include <dirent.h>
#include <jpeglib.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include "texture.h"
#include "plane.h"
#include "sphere.h"
#include "triangle_mesh.h"
#include "tiny_obj_loader.h"
#include "camera.h"
#include "vec3.h"
#include "triangle.h"


#define PI 3.14159265358979323846
#define BIAS 0.0001; 
std::map<std::string, Texture> textures_map;
int WIDTH = 640;
int HEIGHT = 480;

enum ray_type
{
	shadow, camera
};

void save_image(double WIDTH, double HEIGHT, Color* image) 
{
	std::ofstream ofs("./scene.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
	for (int i = 0; i < WIDTH * HEIGHT; ++i) 
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
	for (unsigned int i = 0; i < scene.size(); i++) 
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
				normal = mesh->tri_fnormal;
				Vec3 tex = mesh->tri_tex_coordinates;

				Texture tex_image;

				for (auto x : textures_map) {
					if (mesh->tex_name == x.first) {
						tex_image = x.second;
						break;
					}

				}
				int i = round(tex.x * tex_image.width);
				int j = round(tex.y * tex_image.height);

				color = Color(tex_image.data[(i + j * tex_image.width) * tex_image.channels]/255.,
				 tex_image.data[(i + j * tex_image.width) * tex_image.channels + 1]/255., 
				 tex_image.data[(i + j * tex_image.width) * tex_image.channels + 2]/255.);
				 
				/*//ambient light
				color += texture * 0.05;

				for (Light light : lights)
				{
					Vec3 light_dir = light.get_direction(point);
					double r2 = light_dir.dot_product(light_dir);
					double distance = sqrt(r2);
					light_dir = light_dir.normalize();

					color += texture * light.get_color() * light.get_intensity() / (4 * PI * r2) * std::max(0.0, normal.dot_product(light_dir));
				}
				double result = std::max(0.0, normal.dot_product(ray->get_direction() * -1));
				color += Color(result, result, result);*/
				return color;
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

				color = diffuse * 0 + specular * 1;
				return color;
			}
		}
	}
	return Color();
}

void start_thread(const unsigned start, const unsigned end, Color *image, int th_i)
{
	std::vector<Object*> scene;
	std::vector<Light> lights;

	Camera camera(Vec3(15, 5, 0.0), Vec3(0.0, 5, 0.0), Vec3(15.0, 6, 0.0), ((50 * 0.5) * PI / 180.0), (double)WIDTH/(double)HEIGHT);

	TriangleMesh* mesh = new TriangleMesh("horse.obj", Color(1.0, 0.0, 0.0), diffuse);
	scene.push_back(mesh);


  for (unsigned i = start; i < end; i++) {
		int x = i % WIDTH;
		int y = i / WIDTH;
		double new_x = (2.0 * x) / WIDTH - 1.0;
		double new_y = (-2.0 * y) / HEIGHT + 1.0;
		Color* pixel = image + (x + y * WIDTH);

		Ray* camera_ray = camera.create_camera_ray(new_x, new_y);
		*pixel = cast_ray(camera_ray, scene, lights);
	}
	
	std::cout << "thread " << th_i << "is finished" << std::endl;
}

void create_threads(Color *image) 
{
	int no_threads = std::thread::hardware_concurrency();
	std::cout << "Resolution: " << WIDTH << "x" << HEIGHT << std::endl;
	std::cout << "Threads: " << no_threads << std::endl;

	std::thread *thread_pool = new std::thread[no_threads];

	int resolution = WIDTH * HEIGHT;

	int chunk = resolution / no_threads;
	int rem = resolution % no_threads;

	// launch threads
	for (int i = 0; i < no_threads - 1; i++) 
	{
		thread_pool[i] = std::thread(start_thread, i * chunk, (i + 1) * chunk, image, i);
	}

	start_thread((no_threads - 1) * chunk, (no_threads)*chunk + rem, image, no_threads-1);

	for (int i = 0; i < no_threads - 1; i++) thread_pool[i].join();

	save_image(WIDTH, HEIGHT, image);
}

void setup() 
{
	Color* image = new Color[WIDTH * HEIGHT];

    std::string path = "textures";
    for (const auto & entry : fs::directory_iterator(path)) 
	{
		std::string name = entry.path().string();
		Texture texture(name.c_str());
		textures_map.insert(std::pair<std::string, Texture>(name, texture));
	}
        
	create_threads(image);
}


int main() 
{
	auto timeStart = std::chrono::high_resolution_clock::now();
	setup();
	auto timeEnd = std::chrono::high_resolution_clock::now();
	auto passedTime = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
	fprintf(stderr, "\rDone: %.2f (sec)\n", passedTime / 1000);
	return 0;
}