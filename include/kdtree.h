// Theory and Implementation insipred by 
// http://www.pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Kd-Tree_Accelerator.html
#ifndef KDTREE_h
#define KDTREE_h
#include <object.h>
#include <vector>
#include <tree.h>
#include <cstring>
#include <algorithm>
#include <limits.h>

enum class edge_type 
{
    start, end
};

struct bound_edge 
{
    double t;
    int prim_index;
    edge_type type;

    bound_edge() {}
    bound_edge(double t, int prim_index, bool starting): t(t), prim_index(prim_index) 
    {
        type = starting ? edge_type::start : edge_type::end; 
    }
}; 

struct KD_node 
{
    union {
        double split;                 
        int one_primitive;            
        int prim_indices_offset;  
    };
    union {
        int flags;        
        int prims_count;        
        int above_child;   
    };
    
    void init_leaf(int *p_count, int np, std::vector<int> *prim_indices)
    {
        flags = 3;
        prims_count |= (np << 2);
        if (np == 0) one_primitive = 0;
        else if (np == 1) one_primitive = p_count[0];
        else 
        {
            prim_indices_offset = prim_indices->size();
            for (int i = 0; i < np; ++i)
                prim_indices->push_back(p_count[i]);
        }
    }
    void init_interior(int axis, int ac, double s) 
    {
        split = s;
        flags = axis;
        above_child |= (ac << 2);
    }
    double get_split_pos() const { return split; }
    int get_prims_count() const { return prims_count >> 2; }
    int get_split_axis() const { return flags & 3; }
    bool is_leaf() const { return (flags & 3) == 3; }
    int get_above_child() const { return above_child >> 2; }
};

struct KD_todo 
{
    const KD_node *node;
    double tmin, tmax;
};

class KDtree : public Tree
{
private:
    const int isect_cost, traversal_cost, max_prims;
    const double empty_bonus;
    std::vector<std::shared_ptr<Object>> primitives;
    std::vector<int> prims_indices;
    KD_node *nodes;
    int nodes_count, next_free_node;
    BBOX bounds;

    void build_tree(int node_index, const BBOX &bounds, const std::vector<BBOX> &prim_bounds, int *prim_nums, int prims_count,
         int depth, const std::unique_ptr<bound_edge[]> edges[3], int *prims0, int *prims1, int bad_refines = 0);

public:
    KDtree(const std::vector<std::shared_ptr<Object>> &prims, int isect_cost = 80, int traversal_cost = 1, 
        double empty_bonus = 0.5, int max_prims = 1, int max_depth = -1);
    bool intersect_tree(std::shared_ptr<Ray> ray) override;
};

#endif