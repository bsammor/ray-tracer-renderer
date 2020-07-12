#include <vector>
#include <tree.h>
#include <object.h>

class Octree : public Tree
{
private:
    Octree *children[8];
    BBOX bounds;
    std::vector<std::shared_ptr<Object>> primitives;
    const static int max_depth = 8;
    const static int min_prims = 1;

    int first_node(double tx0, double ty0, double tz0, double txm, double tym, double tzm);
    int new_node(double txm, int x, double tym, int y, double tzm, int z);
    void proc_subtree(double tx0, double ty0, double tz0, double tx1, double ty1, double tz1, Octree *node, std::vector<Octree*> &hit_nodes, std::shared_ptr<Ray> ray);

public:
    Octree(const std::vector<std::shared_ptr<Object>> p, BBOX b, int depth = 0);
    bool intersect_tree(std::shared_ptr<Ray> ray) override;
};