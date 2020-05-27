#ifndef TREE_H
#define TREE_H
#include <memory>
#include <ray.h>

class Tree
{
public:
    virtual bool intersect_tree(std::shared_ptr<Ray> ray) const = 0;
};

#endif