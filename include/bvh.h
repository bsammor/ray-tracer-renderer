#include <box.h>
#include <vector>
#include <triangle.h>

struct BVHNode
{
    BVHNode* left;
    BVHNode* right;
    Box bounds;
    bool isLeaf;
    std::vector<Triangle*> primitives;
};