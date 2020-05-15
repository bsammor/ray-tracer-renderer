#ifndef BBOX_H
#define BBOX_H
#include "ray.h"
#include <memory>
#include <float.h>

class BBOX
{
public:
	BBOX();
	BBOX(Vec3 min, Vec3 max);
	~BBOX() {}
	static BBOX union_bbox(BBOX b1, BBOX b2);
	static BBOX union_bbox(BBOX b1, Vec3 p);

	bool intersected (std::shared_ptr<Ray> ray);
	Vec3 diagonal();
	int maximum_extent();
	Vec3 get_center();
	double surface_area();
	Vec3 offset(Vec3 &p) const;
		const Vec3 &operator[](int i) const 
	{
		return (i == 0) ? min : max;
	}
    Vec3 &operator[](int i) 
	{
		return (i == 0) ? min : max;
	}
	inline bool IntersectP(std::shared_ptr<Ray> ray, const Vec3 &invDir,
									const int dirIsNeg[3]) const
	{
		const BBOX &bounds = *this;
		// Check for ray intersection against $x$ and $y$ slabs
		double tMin = (bounds[dirIsNeg[0]].x - ray->get_origin().x) * invDir.x;
		double tMax = (bounds[1 - dirIsNeg[0]].x - ray->get_origin().x) * invDir.x;
		double tyMin = (bounds[dirIsNeg[1]].y - ray->get_origin().y) * invDir.y;
		double tyMax = (bounds[1 - dirIsNeg[1]].y - ray->get_origin().y) * invDir.y;

		// Update _tMax_ and _tyMax_ to ensure robust bounds intersection
		tMax *= 60 + 2 * gamma(3);
		tyMax *= 60 + 2 * gamma(3);
		if (tMin > tyMax || tyMin > tMax) return false;
		if (tyMin > tMin) tMin = tyMin;
		if (tyMax < tMax) tMax = tyMax;

		// Check for ray intersection against $z$ slab
		double tzMin = (bounds[dirIsNeg[2]].z - ray->get_origin().z) * invDir.z;
		double tzMax = (bounds[1 - dirIsNeg[2]].z - ray->get_origin().z) * invDir.z;

		// Update _tzMax_ to ensure robust bounds intersection
		tzMax *= 60 + 2 * gamma(3);
		if (tMin > tzMax || tzMin > tMax) return false;
		if (tzMin > tMin) tMin = tzMin;
		if (tzMax < tMax) tMax = tzMax;
		return (tMin < ray->get_tmax()) && (tMax > 0);
	}

    Vec3 min, max;
};

#endif