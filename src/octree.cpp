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

bool Inside(BBOX b1, BBOX b2) {
    bool x = (b1.max.x >= b2.min.x) && (b1.min.x <= b2.max.x);
    bool y = (b1.max.y >= b2.min.y) && (b1.min.y <= b2.max.y);
    bool z = (b1.max.z >= b2.min.z) && (b1.min.z <= b2.max.z);
    return (x && y && z);
}

void get_children_bbox(std::vector<BBOX> &children_bounds, BBOX parent_bounds)
{
    Vec3 min = parent_bounds.min;
    Vec3 max = parent_bounds.max;
    Vec3 center = parent_bounds.get_center();

    Vec3 corners[8] = {Vec3(min.x, min.y, min.z), Vec3(min.x, min.y, max.z), Vec3(min.x, max.y, min.z), Vec3(min.x, max.y, max.z), 
        Vec3(max.x, min.y, min.z), Vec3(max.x, min.y, max.z), Vec3(max.x, max.y, min.z), Vec3(max.x, max.y, max.z)};

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
                    if (children[i]->intersect_tree(ray))
                        hit = true;
            }
        }
        else if (primitives.size() > 0)
        {
            double u,v,t;
            for (unsigned i = 0 ; i < primitives.size(); i++)
            {
                ray->intersections++;
                if (primitives[i]->intersected(ray, i, u, v, t))
                    hit = true;
            }
        }
    }

    return hit;
}

using namespace std;

unsigned char a; // because an unsigned char is 8 bits

int first_node(double tx0, double ty0, double tz0, double txm, double tym, double tzm)
{
    unsigned char answer = 0;   // initialize to 00000000
    // select the entry plane and set bits
    if(tx0 > ty0){
        if(tx0 > tz0){ // PLANE YZ
            if(tym < tx0) answer|=2;    // set bit at position 1
            if(tzm < tx0) answer|=1;    // set bit at position 0
            return (int) answer;
        }
    }
    else {
        if(ty0 > tz0){ // PLANE XZ
            if(txm < ty0) answer|=4;    // set bit at position 2
            if(tzm < ty0) answer|=1;    // set bit at position 0
            return (int) answer;
        }
    }
    // PLANE XY
    if(txm < tz0) answer|=4;    // set bit at position 2
    if(tym < tz0) answer|=2;    // set bit at position 1
    return (int) answer;
}

int new_node(double txm, int x, double tym, int y, double tzm, int z)
{
    if(txm < tym){
        if(txm < tzm){return x;}  // YZ plane
    }
    else{
        if(tym < tzm){return y;} // XZ plane
    }
    return z; // XY plane;
}

void proc_subtree (double tx0, double ty0, double tz0, double tx1, double ty1, double tz1, Octree *node, std::shared_ptr<Ray> ray, bool &hit)
{
    float txm, tym, tzm;
    int currNode;

    if(tx1 < 0 || ty1 < 0 || tz1 < 0) return;
    if(node->children[0] == NULL){
        for (int i = 0; i < node->primitives.size(); i++)
        {
            double u, v, t;
            if (node->primitives[i]->intersected(ray, i, u, v, t))
                hit = true;
        }
        return;
    }

    txm = 0.5*(tx0 + tx1);
    tym = 0.5*(ty0 + ty1);
    tzm = 0.5*(tz0 + tz1);

    currNode = first_node(tx0,ty0,tz0,txm,tym,tzm);
    do{
        switch (currNode)
        {
        case 0: { 
            proc_subtree(tx0,ty0,tz0,txm,tym,tzm,node->children[a], ray, hit);
            currNode = new_node(txm,4,tym,2,tzm,1);
            break;}
        case 1: { 
            proc_subtree(tx0,ty0,tzm,txm,tym,tz1,node->children[1^a], ray, hit);
            currNode = new_node(txm,5,tym,3,tz1,8);
            break;}
        case 2: { 
            proc_subtree(tx0,tym,tz0,txm,ty1,tzm,node->children[2^a], ray, hit);
            currNode = new_node(txm,6,ty1,8,tzm,3);
            break;}
        case 3: { 
            proc_subtree(tx0,tym,tzm,txm,ty1,tz1,node->children[3^a], ray, hit);
            currNode = new_node(txm,7,ty1,8,tz1,8);
            break;}
        case 4: { 
            proc_subtree(txm,ty0,tz0,tx1,tym,tzm,node->children[4^a], ray, hit);
            currNode = new_node(tx1,8,tym,6,tzm,5);
            break;}
        case 5: { 
            proc_subtree(txm,ty0,tzm,tx1,tym,tz1,node->children[5^a], ray, hit);
            currNode = new_node(tx1,8,tym,7,tz1,8);
            break;}
        case 6: { 
            proc_subtree(txm,tym,tz0,tx1,ty1,tzm,node->children[6^a], ray, hit);
            currNode = new_node(tx1,8,ty1,8,tzm,7);
            break;}
        case 7: { 
            proc_subtree(txm,tym,tzm,tx1,ty1,tz1,node->children[7^a], ray, hit);
            currNode = 8;
            break;}
        }
    } while (currNode<8);
}

bool Octree::ray_octree_traversal(std::shared_ptr<Ray> ray)
{
    a = 0;
    bool hit = false;

    Vec3 size = bounds.diagonal();
    
    if(ray->get_direction()[0] < 0)
    {
        a |= 4 ; //bitwise OR (latest bits are XYZ)
    }
    if(ray->get_direction()[1] < 0)
    {
        a |= 2 ; 
    }
    if(ray->get_direction()[2] < 0)
    {
        a |= 1 ; 
    }
    Vec3 invDir = Vec3(1 / ray->get_direction().x, 1 / ray->get_direction().y, 1 / ray->get_direction().z); 
    int dirIsNeg[3];
    dirIsNeg[0] = (invDir.x < 0); 
    dirIsNeg[1] = (invDir.y < 0); 
    dirIsNeg[2] = (invDir.z < 0); 
    double tx0 = (bounds[dirIsNeg[0]].x - ray->get_origin().x) * invDir.x;
    double tx1 = (bounds[1 - dirIsNeg[0]].x - ray->get_origin().x) * invDir.x;
    double ty0 = (bounds[dirIsNeg[1]].y - ray->get_origin().y) * invDir.y;
    double ty1 = (bounds[1 - dirIsNeg[1]].y - ray->get_origin().y) * invDir.y;
	double tz0 = (bounds[dirIsNeg[2]].z - ray->get_origin().z) * invDir.z;
	double tz1 = (bounds[1 - dirIsNeg[2]].z - ray->get_origin().z) * invDir.z;

	if (max(max(tx0,ty0),tz0) < min(min(tx1,ty1),tz1) )
	{
      proc_subtree(tx0,ty0,tz0,tx1,ty1,tz1,this, ray, hit);
    }
    return hit;
}