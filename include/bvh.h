// Theory and Implementation insipred by 
// http://www.pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies.html
#ifndef BVH_H
#define BVH_H
#include <vector>
#include <object.h>
#include <tree.h>
#include <algorithm>

struct bucket_info 
{
    int count = 0;
    BBOX bounds;
};

struct BVH_node
{
    BBOX bounds;
    BVH_node *children[2];
    int axis, first_prim_offset, prims_count;

    void init_leaf(int first, int num, const BBOX &b)
    {
        bounds = b;
        first_prim_offset = first;
        prims_count = num;
        children[0] = children[1] = nullptr;
    }
    void init_interior(int dim, BVH_node *c0, BVH_node *c1)
    {
        axis = dim;
        children[0] = c0;
        children[1] = c1;
        bounds = BBOX::union_bbox(c0->bounds, c1->bounds);
        prims_count = 0;
    }
};

struct prim_info
{
    size_t prim_index;
    BBOX bounds;
    Vec3 center;

    prim_info(size_t prim_index, BBOX &bounds) : prim_index(prim_index), bounds(bounds), center(bounds.get_center()) { }
};

struct linear_BVH_node
{
    uint16_t prims_count;
    uint8_t axis;        
    uint8_t pad[1];
    BBOX bounds;
    union 
    {
        int prims_offset;
        int second_child_offset;
    };        
};

class BVH : public Tree
{
private:
    int min_prims;
    std::vector<std::shared_ptr<Object>> primitives;
    linear_BVH_node *nodes = nullptr;

    BVH_node* build_hierarchy(std::vector<prim_info> &prims_info, int start, int end, int *total_nodes, std::vector<std::shared_ptr<Object>> &ordered_prims);
    int flatten_hierarchy(BVH_node *node, int *offset);

public:
    BVH(std::vector<std::shared_ptr<Object>> &prims, int min_prims);
    bool intersect_tree(std::shared_ptr<Ray> ray) override;
};

#endif