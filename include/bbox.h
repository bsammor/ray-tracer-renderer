// Theory and Implementation insipred by 
// www.scratchapixel.com/lessons/advanced-rendering/introduction-acceleration-structure/bounding-volume
// Physically Based Rendering: From Theory To Implementation, 3rd edition
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes
#ifndef BBOX_H
#define BBOX_H
#include <ray.h>
#include <memory>
#include <globals.h>

class BBOX
{
public:
    Vec3 min, max;
	
	BBOX();
	BBOX(Vec3 min, Vec3 max);
	static BBOX union_bbox(BBOX b1, BBOX b2);
	static BBOX union_bbox(BBOX b1, Vec3 p);

	Vec3 get_center();
	Vec3 diagonal() const;
	double surface_area() const;
	int maximum_extent() const;
	Vec3 offset(Vec3 &p) const;
	const Vec3 &operator[](int i) const {return (i == 0) ? min : max;}
    Vec3 &operator[](int i) {return (i == 0) ? min : max;}
	inline bool intersected(std::shared_ptr<Ray> ray, const Vec3 &inv_dir, const int dir_is_neg[3]) const
	{
		const BBOX &bounds = *this;
		double tmin = (bounds[dir_is_neg[0]].x - ray->get_origin().x) * inv_dir.x;
		double tmax = (bounds[1 - dir_is_neg[0]].x - ray->get_origin().x) * inv_dir.x;
		double tymin = (bounds[dir_is_neg[1]].y - ray->get_origin().y) * inv_dir.y;
		double tymax = (bounds[1 - dir_is_neg[1]].y - ray->get_origin().y) * inv_dir.y;

		if (tmin > tymax || tymin > tmax) return false;
		if (tymin > tmin) tmin = tymin;
		if (tymax < tmax) tmax = tymax;

		double tzmin = (bounds[dir_is_neg[2]].z - ray->get_origin().z) * inv_dir.z;
		double tzmax = (bounds[1 - dir_is_neg[2]].z - ray->get_origin().z) * inv_dir.z;

		if (tmin > tzmax || tzmin > tmax) return false;
		if (tzmin > tmin) tmin = tzmin;
		if (tzmax < tmax) tmax = tzmax;

		return (tmin < ray->tmax) && (tmax > 0);
	}
	inline bool intersected(std::shared_ptr<Ray> ray, double *hitt0, double *hitt1) const 
	{
		double t0 = 0, t1 = ray->tmax;
		for (int i = 0; i < 3; ++i) 
		{
			double inv_ray_dir = 1 / ray->get_direction()[i];
			double tnear = (min[i] - ray->get_origin()[i]) * inv_ray_dir;
			double tfar = (max[i] - ray->get_origin()[i]) * inv_ray_dir;

			if (tnear > tfar) std::swap(tnear, tfar);

			tfar *= 1 + 2 * gamma(3);
			t0 = tnear > t0 ? tnear : t0;
			t1 = tfar < t1 ? tfar : t1;
			if (t0 > t1) return false;
		}
		if (hitt0) *hitt0 = t0;
		if (hitt1) *hitt1 = t1;
		
		return true;
	}
};

#endif