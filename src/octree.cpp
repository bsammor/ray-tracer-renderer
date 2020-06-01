#include <octree.h>

bool is_overlapping(BBOX child_bounds, BBOX tri_bounds)
{
    Vec3 dist = (tri_bounds.min + tri_bounds.max - child_bounds.max - child_bounds.min) * 0.5;
    if(dist.x < 0)
        dist.x = (-1*dist.x);
    if(dist.y < 0)
        dist.y = (-1*dist.y);
    if(dist.z < 0)
        dist.z = (-1*dist.z);

    //this is the sum of both box's radii
    Vec3 sum = (tri_bounds.min - tri_bounds.max + child_bounds.max - child_bounds.min) * 0.5;

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
        child.min = Vec3(std::min(v.x, center.x), std::fmin(v.y, center.y), std::fmin(v.z, center.z));
        child.max = Vec3(std::max(v.x, center.x), std::fmax(v.y, center.y), std::fmax(v.z, center.z));
        children_bounds.push_back(child);
    }
}

Octree::Octree(const std::vector<std::shared_ptr<Object>> p, BBOX b, int &totalprims, int depth)
{
    bounds = b;
    if (p.size() < 1) 
        return;

    if (p.size() <= max_prims || depth >= max_depth)
    {
        this->primitives = p;
        totalprims += p.size();
        std::cout << p.size() << " primitives were added to a leaf" << std::endl;
        return;
    }

    std::vector<BBOX> children_bounds;
    get_children_bbox(children_bounds, bounds);
    
    for (int i = 0; i < 8; i++)
    {
        //figure out which children belong in this child using the bbox from previous step
        BBOX childb = children_bounds[i];
        std::vector<std::shared_ptr<Object>> childp;

        for (unsigned i = 0; i < p.size(); i++)
        {
            if (is_overlapping(childb, p[i]->get_bbox()))
                childp.push_back(p[i]);
        }

        children[i] = new Octree(childp, childb, totalprims, depth+1);
    }
}

bool Octree::intersect_tree(std::shared_ptr<Ray> ray) const
{
    return false;
}