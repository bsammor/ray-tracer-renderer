#ifndef BVH_H
#define BVH_H
#include <vector>
#include <iostream>
#include <memory>
#include <object.h>
#include <bbox.h>
#include <atomic>
#include <algorithm>

struct BVHBuildNode {
    void InitLeaf(int first, int n, const BBOX &b) {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }
    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
        children[0] = c0;
        children[1] = c1;
        bounds = BBOX::union_bbox(c0->bounds, c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }
    BBOX bounds;
    BVHBuildNode *children[2];
    int splitAxis, firstPrimOffset, nPrimitives;
};

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo(size_t primitiveNumber, BBOX &bounds)
        : primitiveNumber(primitiveNumber), bounds(bounds),
          centroid(bounds.min * 0.5 + bounds.max * .5) { }
    size_t primitiveNumber;
    BBOX bounds;
    Vec3 centroid;
};

struct LinearBVHNode {
    BBOX bounds;
    union {
        int primitivesOffset;    // leaf
        int secondChildOffset;   // interior
    };
    uint16_t nPrimitives;  // 0 -> interior node
    uint8_t axis;          // interior node: xyz
    uint8_t pad[1];        // ensure 32 byte total size
};


// BVHAccel Declarations
class BVHAccel {
  public:
    // BVHAccel Public Methods
    BVHAccel(std::vector<std::shared_ptr<Object>> p,
             int maxPrimsInNode = 1);
    BBOX WorldBound() const;
    bool Intersect(std::shared_ptr<Ray> ray) const;
    bool IntersectP(std::shared_ptr<Ray> ray) const;

  private:
    // BVHAccel Private Methods
    BVHBuildNode *recursiveBuild(std::vector<BVHPrimitiveInfo> &primitiveInfo,
        int start, int end, int *totalNodes,
        std::vector<std::shared_ptr<Object>> &orderedPrims);
    int flattenBVHTree(BVHBuildNode *node, int *offset);

    // BVHAccel Private Data
    int maxPrimsInNode;
    std::vector<std::shared_ptr<Object>> primitives;
    LinearBVHNode *nodes = nullptr;
};

#endif