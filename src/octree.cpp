#include <octree.h>

bool is_overlapping(BBOX child_bounds, BBOX tri_bounds)
{
    Vec3 dist = (tri_bounds.max + tri_bounds.min - child_bounds.max - child_bounds.min) * 0.5;
    if(dist.x < 0)
        dist.x = (-1*dist.x);
    if(dist.y < 0)
        dist.y = (-1*dist.y);
    if(dist.z < 0)
        dist.z = (-1*dist.z);

    //this is the sum of both box's radii
    Vec3 sum = (tri_bounds.max - tri_bounds.min + child_bounds.max - child_bounds.min) * 0.5;

    //if the dist is greater than the sum, then they
    //don't overlap
    if(dist.x > sum.x) return false;
    if(dist.y > sum.y) return false;
    if(dist.z > sum.z) return false;

    return true;
}

void get_children_bbox(std::vector<BBOX> &children_bounds, BBOX parent_bounds)
{
    Vec3 min = parent_bounds.min;
    Vec3 max = parent_bounds.max;
    Vec3 center = parent_bounds.get_center();

    Vec3 corners[8] = {Vec3(min.x, min.y, min.z), Vec3(min.x, max.y, min.z), Vec3(min.x, min.y, max.z), Vec3(min.x, max.y, max.z), 
        Vec3(max.x, min.y, min.z), Vec3(max.x, max.y, min.z), Vec3(max.x, min.y, max.z), Vec3(max.x, max.y, max.z)};

    for (Vec3 v : corners)
    {
        BBOX child;
        child.min = Vec3(std::min(v.x, center.x), std::min(v.y, center.y), std::min(v.z, center.z));
        child.max = Vec3(std::max(v.x, center.x), std::max(v.y, center.y), std::max(v.z, center.z));
        children_bounds.push_back(child);
    }
}

Octree::Octree(const std::vector<std::shared_ptr<Object>> p, BBOX b, int &totalprims, int depth)
{
    for (int i = 0; i < 8; i ++)
        children[i] = NULL;
 
    bounds = b;
    if (p.size() < 1) 
        return;

    if (p.size() <= max_prims || depth >= max_depth)
    {
        this->primitives = p;
        totalprims += p.size();
        return;
    }

    std::vector<BBOX> children_bounds;
    get_children_bbox(children_bounds, bounds);
    
    for (int i = 0; i < 8; i++)
    {
        //figure out which children belong in this child using the bbox from previous step
        BBOX childb = children_bounds[i];
        std::vector<std::shared_ptr<Object>> childp;

        for (unsigned j = 0; j < p.size(); j++)
        {
            if (is_overlapping(childb, p[j]->get_bbox()))
                childp.push_back(p[j]);
        }

        children[i] = new Octree(childp, childb, totalprims, depth+1);
    }
}

bool traverse_tree(std::shared_ptr<Ray> ray, const Octree *tree)
{
    bool hit = false;
    Vec3 invdir = Vec3(1 / ray->get_direction().x, 1 / ray->get_direction().y, 1 / ray->get_direction().z); 
    int sign[3];
    sign[0] = (invdir.x < 0); 
    sign[1] = (invdir.y < 0); 
    sign[2] = (invdir.z < 0); 

    if (tree->bounds.IntersectP(ray, invdir, sign))
    {
        if (tree->children[0] != NULL)
        {
            for (int i = 0; i < 8; i++)
            {
                if (tree->children[i]->bounds.IntersectP(ray, invdir, sign))
                    if (traverse_tree(ray, tree->children[i]))
                        hit = true;
            }
        }

        else 
        {
            double u,v,t;
            for (unsigned i = 0 ; i < tree->primitives.size(); i++)
            {

                if (tree->primitives[i]->intersected(ray, i, u, v, t))
                    hit = true;
            }
        }
    }
    return hit;
}

bool Octree::intersect_tree(std::shared_ptr<Ray> ray) const
{
    bool hit = false;
    Vec3 invdir = Vec3(1 / ray->get_direction().x, 1 / ray->get_direction().y, 1 / ray->get_direction().z); 
    int sign[3];
    sign[0] = (invdir.x < 0); 
    sign[1] = (invdir.y < 0); 
    sign[2] = (invdir.z < 0); 
    if (bounds.IntersectP(ray, invdir, sign))
    {
        if (children[0] != NULL)
        {
            for (int i = 0; i < 8; i++)
            {
                if (children[i]->bounds.IntersectP(ray, invdir, sign))
                    if (traverse_tree(ray, children[i]))
                        hit = true;
            }
        }
        else 
        {
            double u,v,t;
            for (unsigned i = 0 ; i < primitives.size(); i++)
            {
                if (primitives[i]->intersected(ray, i, u, v, t))
                    hit = true;
            }
        }
    }

    return hit;
}