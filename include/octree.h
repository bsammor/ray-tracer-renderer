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
    Octree(const std::vector<std::shared_ptr<Object>> p, BBOX b, int &totalprims, int depth = 0);
    ~Octree();
    bool intersect_tree(std::shared_ptr<Ray> ray) const;
    bool ray_octree_traversal(std::shared_ptr<Ray> ray);

    Octree *children[8];
    BBOX bounds;
    std::vector<std::shared_ptr<Object>> primitives;

private:
    const static int max_depth = 5;
    const static int max_prims = 1;
};