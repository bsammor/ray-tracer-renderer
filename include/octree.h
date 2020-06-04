#include <iostream>
#include <vec3.h>
#include <bbox.h>
#include <vector>
#include <memory>
#include <tree.h>
#include <object.h>

//TODO: FIX is_overlapping() function. 
//Triangles are being thrown even though they should be inside one of the boxes.

class Octree : public Tree
{
public:
    Octree(const std::vector<std::shared_ptr<Object>> p, BBOX b, int &totalprims, int depth = 0);
    ~Octree();
    bool intersect_tree(std::shared_ptr<Ray> ray) const;

    Octree *children[8];
    BBOX bounds;
    std::vector<std::shared_ptr<Object>> primitives;

private:
    const static int max_depth = 5;
    const static int max_prims = 1;
};