#ifndef NAIVE_TRACER_H
#define NAIVE_TRACER_H
extern std::map<std::string,Texture> textures_map;

std::shared_ptr<Ray> create_reflection_ray(std::shared_ptr<Ray> ray, std::shared_ptr<Object> obj) 
{
	Vec3 point = ray->get_intersection_point();
	Vec3 incidence = ray->get_direction();
	Vec3 normal = obj->get_normal(point);
	Vec3 direction = incidence - (normal * 2 * incidence.dot_product(normal));

	return std::shared_ptr<Ray>(new Ray(point, direction.normalize(), minimum, infinity));
}

Vec3 refract(Vec3 I, Vec3 N, double ior)
{
	double cosi = clamp(-1, 1, I.dot_product(N));
	double etai = 1, etat = ior;
	Vec3 n = N;

	if (cosi < 0) cosi = -cosi;
	else 
	{
		std::swap(etai, etat); 
		n = (N * -1);
	}

	double eta = etai / etat;
	double k = 1 - eta * eta * (1 - cosi * cosi);

	return k < 0 ? Vec3() : I * eta  + n * (eta * cosi - sqrt(k));
}

double fresnel(Vec3& I, Vec3& N, double ior)
{
	double kr = 0;
	double cosi = clamp(-1, 1, I.dot_product(N));
	double etai = 1, etat = ior;
	if (cosi > 0) std::swap(etai, etat);
	
	double sint = etai / etat * sqrtf(std::max(0.0, 1 - cosi * cosi));

	if (sint >= 1) kr = 1;
	else 
	{
		double cost = sqrtf(std::max(0.0, 1 - sint * sint));
		cosi = fabsf(cosi);
		double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}

	return kr;
}

bool trace(std::shared_ptr<Ray> ray, std::vector<std::shared_ptr<Object>> scene, ray_type type)
{
	bool intersected = false;

	for (unsigned int i = 0; i < scene.size(); i++) 
	{
		if (scene[i]->intersected(ray, i))
		{
			if (type == secondary && scene[i]->get_material() == reflective_refractive) continue;
			intersected = true;
		}
	}

	return intersected;
}

inline Color cast_ray(std::shared_ptr<Ray> ray, std::vector<std::shared_ptr<Object>> scene, std::vector<Light> lights, int depth = 0) 
{
	if (depth < 5) 
	{
		if (trace(ray, scene, primary))
		{
			int obj_index = ray->obj_index;
			Color color;
			Vec3 point = ray->get_intersection_point();
			Vec3 normal = scene[obj_index]->get_normal(point);

		 	if (scene[obj_index]->get_material() == diffuse)
			{
				Color obj_color = scene[obj_index]->get_color();

				if (!ray->tex.empty())
				{
					Texture tex_image;
					Vec3 tex = ray->vt0 * (1 - ray->u - ray->v) + ray->vt1 * ray->u + ray->vt2 * ray->v;

					for (auto x : textures_map) 
					{
						if (ray->tex == x.first) 
						{
							tex_image = x.second;
							break;
						}
					}
					int i = round(tex.x * tex_image.width);
					int j = round(tex.y * tex_image.height);

					obj_color = Color(tex_image.data[(i + j * tex_image.width) * tex_image.channels]/255.,
					tex_image.data[(i + j * tex_image.width) * tex_image.channels + 1]/255., 
					tex_image.data[(i + j * tex_image.width) * tex_image.channels + 2]/255.);
				}

				//ambient light
				color += obj_color * 0.2;

				for (Light light : lights) 
				{
					Vec3 light_dir = light.get_direction(point);
					double r2 = light_dir.dot_product(light_dir);
					double distance = sqrt(r2);
					light_dir = light_dir.normalize();

					std::shared_ptr<Ray> shadow_ray = std::shared_ptr<Ray>(new Ray(point + normal * BIAS, light_dir, minimum, distance));
					bool covered = !trace(shadow_ray, scene, secondary);
					color += (obj_color * light.get_color() * light.get_intensity() / (4 * M_PI * r2) * std::max(0.0, normal.dot_product(light_dir))) * covered;
				}
				
				return color;
			}
			else if (scene[obj_index]->get_material() == reflective)
			{
				std::shared_ptr<Ray> reflection_ray = create_reflection_ray(ray, scene[obj_index]);
				color += cast_ray(reflection_ray, scene, lights, depth + 1);

				return color;
			}
			else if (scene[obj_index]->get_material() == reflective_refractive)
			{
				Color refraction_color, reflection_color;
				double kr;
				Vec3 dir = ray->get_direction();

				kr = fresnel(dir, normal, scene[obj_index]->get_ior());
				bool outside = dir.dot_product(normal) < 0;
				Vec3 bias = normal * BIAS;

				if (kr < 1)
				{
					Vec3 refraction_direction = refract(dir, normal, scene[obj_index]->get_ior()).normalize();
					Vec3 refrection_origin = outside ? point - bias : point + bias;

					std::shared_ptr<Ray> refraction_ray = std::shared_ptr<Ray>(new Ray(refrection_origin, refraction_direction, minimum, infinity));
					refraction_color = cast_ray(refraction_ray, scene, lights, depth + 1);
				}

				std::shared_ptr<Ray> reflection_ray = create_reflection_ray(ray, scene[obj_index]);
				Vec3 reflection_direction = reflection_ray->get_direction();
				Vec3 reflection_origin = outside ? point + bias : point - bias;
				std::shared_ptr<Ray> test = std::shared_ptr<Ray>(new Ray(reflection_origin, reflection_direction, minimum, infinity));
				reflection_color = cast_ray(test, scene, lights, depth + 1);

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


					std::shared_ptr<Ray> shadow_ray = std::shared_ptr<Ray>(new Ray(point + normal * BIAS, light_dir, minimum, distance));
					bool covered = !trace(shadow_ray, scene, secondary);

					diffuse += (scene[obj_index]->get_color() * light.get_color() * light.get_intensity() / (4 * M_PI * r2) * std::max(0.0, normal.dot_product(light_dir))) * covered;

					Vec3 I = light_dir * -1;
					Vec3 R = I - normal * 2  * I.dot_product(normal);

					 specular += light.get_color() * light.get_intensity() / (4 * M_PI * r2) * std::pow(std::max(0.0, R.dot_product(ray->get_direction() * -1)), 10) * covered;
				}

				color = diffuse * 0 + specular * 1;

				return color;
			}
		}
	}

	return Color();
}

#endif