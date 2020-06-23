#include <iostream>
#include <vec3.h>
#include <bbox.h>
#include <vector>
#include <memory>
#include <tree.h>
#include <object.h>

class Octree : public Tree
{
public:
    Octree(const std::vector<std::shared_ptr<Object>> p, BBOX b, int depth = 0);
    ~Octree();
    bool intersect_tree(std::shared_ptr<Ray> ray);
    int first_node(double tx0, double ty0, double tz0, double txm, double tym, double tzm);
    int new_node(double txm, int x, double tym, int y, double tzm, int z);
    void proc_subtree (double tx0, double ty0, double tz0, double tx1, double ty1, double tz1, Octree *node, std::vector<Octree*> &hit_nodes, std::shared_ptr<Ray> ray);

    Octree *children[8];
    BBOX bounds;
    std::vector<std::shared_ptr<Object>> primitives;

private:
    const static int max_depth = 7;
    const static int min_prims = 1;
};