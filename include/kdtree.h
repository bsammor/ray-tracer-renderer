#ifndef KDTREE_h
#define KDTREE_h
#include <iostream>
#include <memory>
#include <object.h>
#include <vector>
#include <plane.h>

enum class EdgeType { Start, End };
struct BoundEdge {
       BoundEdge() { }
       BoundEdge(double t, int primNum, bool starting)
           : t(t), primNum(primNum) {
           type = starting ? EdgeType::Start : EdgeType::End; 
       }

    double t;
    int primNum;
    EdgeType type;
}; 

struct KdAccelNode {
    void InitLeaf(int *primNums, int np, std::vector<int> *primitiveIndices)
    {
        flags = 3;
        nPrims |= (np << 2);
        if (np == 0)
            onePrimitive = 0;
        else if (np == 1)
            onePrimitive = primNums[0];
        else {
            primitiveIndicesOffset = primitiveIndices->size();
            for (int i = 0; i < np; ++i)
            primitiveIndices->push_back(primNums[i]);
        }
    }
    void InitInterior(int axis, int ac, double s) {
        split = s;
        flags = axis;
        aboveChild |= (ac << 2);
    }
    double SplitPos() const { return split; }
    int nPrimitives() const { return nPrims >> 2; }
    int SplitAxis() const { return flags & 3; }
    bool IsLeaf() const { return (flags & 3) == 3; }
    int AboveChild() const { return aboveChild >> 2; }

    union {
        double split;                  // Interior
        int onePrimitive;             // Leaf
        int primitiveIndicesOffset;   // Leaf
    };
    union {
        int flags;         // Both
        int nPrims;        // Leaf
        int aboveChild;    // Interior
    };
};

struct KdToDo {
    const KdAccelNode *node;
    double tMin, tMax;
};

class KdTreeAccel
{
public:
       KdTreeAccel(const std::vector<std::shared_ptr<Object>> &p,
               int isectCost = 80, int traversalCost = 1, double emptyBonus = 0.5,
               int maxPrims = 1, int maxDepth = -1);
       BBOX WorldBound() const { return bounds; }
       ~KdTreeAccel();
       bool Intersect(std::shared_ptr<Ray> ray) const;
       bool IntersectP(std::shared_ptr<Ray> ray) const;

public:
void buildTree(int nodeNum, const BBOX &bounds,    
        const std::vector<BBOX> &primBounds, int *primNums, int nprims, int depth,
        const std::unique_ptr<BoundEdge[]> edges[3], int *prims0, int *prims1, int badRefines = 0);
    const int isectCost, traversalCost, maxPrims;
    const double emptyBonus;
    std::vector<std::shared_ptr<Object>> primitives;
    std::vector<int> primitiveIndices;
    KdAccelNode *nodes;
    int nAllocedNodes, nextFreeNode;
    BBOX bounds;

};

#endif