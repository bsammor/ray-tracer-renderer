#include <kdtree.h>
#include <algorithm>
#include <limits.h>
#include <cstring>
static constexpr double Infinity = std::numeric_limits<double>::infinity();

static unsigned int Log2Int (unsigned int val) 
{
    if (val == 0) return UINT_MAX;
    if (val == 1) return 0;
    unsigned int ret = 0;
    while (val > 1) {
        val >>= 1;
        ret++;
    }
    return ret;
}

KdTreeAccel::KdTreeAccel(
        const std::vector<std::shared_ptr<Object>> &p,
        int isectCost, int traversalCost, double emptyBonus,
        int maxPrims, int maxDepth)
    : isectCost(isectCost), traversalCost(traversalCost),
      maxPrims(maxPrims), emptyBonus(emptyBonus), primitives(p) 
{
       nextFreeNode = nAllocedNodes = 0;
       if (maxDepth <= 0)
           maxDepth = std::round(8 + 1.3f * Log2Int(primitives.size()));
      // <<Compute bounds for kd-tree construction>> 
          std::vector<BBOX> primBounds;
          for (const std::shared_ptr<Object> &prim : primitives) 
          {
              BBOX b = prim->get_bbox();
              bounds = BBOX::union_bbox(bounds, b);
              primBounds.push_back(b);
          }

      // <<Allocate working memory for kd-tree construction>> 
          std::unique_ptr<BoundEdge[]> edges[3];
          for (int i = 0; i < 3; ++i)
              edges[i].reset(new BoundEdge[2 * primitives.size()]);
          std::unique_ptr<int[]> prims0(new int[primitives.size()]);
          std::unique_ptr<int[]> prims1(new int[(maxDepth+1) * primitives.size()]);

      // <<Initialize primNums for kd-tree construction>> 
          std::unique_ptr<int[]> primNums(new int[primitives.size()]);
          for (size_t i = 0; i < primitives.size(); ++i)
              primNums[i] = i;

      // <<Start recursive construction of kd-tree>> 
          buildTree(0, bounds, primBounds, primNums.get(), primitives.size(), 
                    maxDepth, edges, prims0.get(), prims1.get());
}


void KdTreeAccel::buildTree(int nodeNum, const BBOX &nodeBounds,
        const std::vector<BBOX> &allPrimBounds, int *primNums,
        int nPrimitives, int depth,
        const std::unique_ptr<BoundEdge[]> edges[3], 
        int *prims0, int *prims1, int badRefines) 
{
    //<<Get next free node from nodes array>> 
       if (nextFreeNode == nAllocedNodes) {
           int nNewAllocNodes = std::max(2 * nAllocedNodes, 512);
           KdAccelNode *n = new KdAccelNode[nNewAllocNodes];
           if (nAllocedNodes > 0) {
               memcpy(n, nodes, nAllocedNodes * sizeof(KdAccelNode));
               free(nodes);
           }
           nodes = n;
           nAllocedNodes = nNewAllocNodes;
       }
       ++nextFreeNode;

    //<<Initialize leaf node if termination criteria met>> 
       if (nPrimitives <= maxPrims || depth == 0) {
           nodes[nodeNum].InitLeaf(primNums, nPrimitives, &primitiveIndices);
           return;
       }

    //<<Initialize interior node and continue recursion>> 
       //<<Choose split axis position for interior node>> 
          int bestAxis = -1, bestOffset = -1;
          double bestCost = Infinity;
          double oldCost = isectCost * double(nPrimitives);
          double totalSA = nodeBounds.surface_area();
          double invTotalSA = 1 / totalSA;
          Vec3 d = nodeBounds.max - nodeBounds.min;
          //Choose which axis to split along>> 
             int axis = nodeBounds.MaximumExtent();

          int retries = 0;
          retrySplit:
          //<<Initialize edges for axis>> 
             for (int i = 0; i < nPrimitives; ++i) {
                 int pn = primNums[i];
                 const BBOX &bounds = allPrimBounds[pn];
                 edges[axis][2 * i] =     BoundEdge(bounds.min[axis], pn, true);
                 edges[axis][2 * i + 1] = BoundEdge(bounds.max[axis], pn, false);
             }
            // <<Sort edges for axis>> 
                std::sort(&edges[axis][0], &edges[axis][2*nPrimitives],
                    [](const BoundEdge &e0, const BoundEdge &e1) -> bool {
                        if (e0.t == e1.t)
                            return (int)e0.type < (int)e1.type;
                        else return e0.t < e1.t; 
                    });

          //<<Compute cost of all splits for axis to find best>> 
             int nBelow = 0, nAbove = nPrimitives;
             for (int i = 0; i < 2 * nPrimitives; ++i) {
                 if (edges[axis][i].type == EdgeType::End) --nAbove;
                 double edgeT = edges[axis][i].t;
                 if (edgeT > nodeBounds.min[axis] &&
                     edgeT < nodeBounds.max[axis]) {
                     //<<Compute cost for split at ith edge>> 
                        //<<Compute child surface areas for split at edgeT>> 
                               int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
                           double belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                                (edgeT - nodeBounds.min[axis]) * 
                                                (d[otherAxis0] + d[otherAxis1]));
                           double aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                                (nodeBounds.max[axis] - edgeT) * 
                                                (d[otherAxis0] + d[otherAxis1]));

                        double pBelow = belowSA * invTotalSA; 
                        double pAbove = aboveSA * invTotalSA;
                        double eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0;
                        double cost = traversalCost + 
                                     isectCost * (1 - eb) * (pBelow * nBelow + pAbove * nAbove);
                        //<<Update best split if this is lowest cost so far>> 
                          if (cost < bestCost)  {
                               bestCost = cost;
                               bestAxis = axis;
                               bestOffset = i;
                           }

                 }
                 if (edges[axis][i].type == EdgeType::Start) ++nBelow;
             }
             

       //<<Create leaf if no good splits were found>> 
          if (bestAxis == -1 && retries < 2) {
              ++retries;
              axis = (axis + 1) % 3;
              goto retrySplit;
          }
          if (bestCost > oldCost) ++badRefines;
          if ((bestCost > 4 * oldCost && nPrimitives < 16) || 
              bestAxis == -1 || badRefines == 3) {
              nodes[nodeNum].InitLeaf(primNums, nPrimitives, &primitiveIndices);
              return; 
          }

       //<<Classify primitives with respect to split>> 
          int n0 = 0, n1 = 0;
          for (int i = 0; i < bestOffset; ++i)
              if (edges[bestAxis][i].type == EdgeType::Start)
                  prims0[n0++] = edges[bestAxis][i].primNum;
          for (int i = bestOffset + 1; i < 2 * nPrimitives; ++i)
              if (edges[bestAxis][i].type == EdgeType::End)
                  prims1[n1++] = edges[bestAxis][i].primNum;

       //<<Recursively initialize children nodes>> 
          double tSplit = edges[bestAxis][bestOffset].t;
          BBOX bounds0 = nodeBounds, bounds1 = nodeBounds;
          bounds0.max[bestAxis] = bounds1.min[bestAxis] = tSplit;
          buildTree(nodeNum + 1, bounds0, allPrimBounds, prims0, n0,
                    depth - 1, edges, prims0, prims1 + nPrimitives, badRefines);
          int aboveChild = nextFreeNode;
          nodes[nodeNum].InitInterior(bestAxis, aboveChild, tSplit);
          buildTree(aboveChild, bounds1, allPrimBounds, prims1, n1, 
                    depth - 1, edges, prims0, prims1 + nPrimitives, badRefines);
}

bool KdTreeAccel::intersect_tree(std::shared_ptr<Ray> ray) const {
    //<<Compute initial parametric range of ray inside kd-tree extent>> 
       double tMin, tMax;
       if (!bounds.IntersectP(ray, &tMin, &tMax)) 
           return false;
       

    //<<Prepare to traverse kd-tree for ray>> 
       Vec3 invDir(1 / ray->get_direction().x, 1 / ray->get_direction().y, 1 / ray->get_direction().z);
       constexpr int maxTodo = 64;
       KdToDo todo[maxTodo];
       int todoPos = 0;

    //<<Traverse kd-tree nodes in order for ray>> 
       bool hit = false;
       const KdAccelNode *node = &nodes[0];
       while (node != nullptr) {
           //<<Bail out if we found a hit closer than the current node>> 
              if (ray->get_tmax() < tMin) break;

           if (!node->IsLeaf()) {
               //<<Process kd-tree interior node>> 
                  //<<Compute parametric distance along ray to split plane>> 
                     int axis = node->SplitAxis();
                     double tPlane = (node->SplitPos() - ray->get_origin()[axis]) * invDir[axis];

                  //<<Get node children pointers for ray>> 
                     const KdAccelNode *firstChild, *secondChild;
                     int belowFirst = (ray->get_origin()[axis] <  node->SplitPos()) ||
                                      (ray->get_origin()[axis] == node->SplitPos() && ray->get_direction()[axis] <= 0);
                     if (belowFirst) {
                         firstChild = node + 1;
                         secondChild = &nodes[node->AboveChild()];
                     } else {
                         firstChild = &nodes[node->AboveChild()];
                         secondChild = node + 1;
                     }

                  //<<Advance to next child node, possibly enqueue other child>> 
                     if (tPlane > tMax || tPlane <= 0)
                         node = firstChild;
                     else if (tPlane < tMin)
                         node = secondChild;
                     else {
                         //<<Enqueue secondChild in todo list>> 
                            todo[todoPos].node = secondChild;
                            todo[todoPos].tMin = tPlane;
                            todo[todoPos].tMax = tMax;
                            ++todoPos;

                         node = firstChild;
                         tMax = tPlane;
                     }

           } else {
               //<<Check for intersections inside leaf node>> 
                  int nPrimitives = node->nPrimitives();
                  if (nPrimitives == 1) {
                      const std::shared_ptr<Object> &p = primitives[node->onePrimitive];
                      //<<Check one primitive inside leaf node>> 
                         double u, v, t;
                         ray->intersections++;
                         if (p->intersected(ray, node->onePrimitive, u, v, t)) 
                             hit = true;

                  } else {
                      for (int i = 0; i < nPrimitives; ++i) {
                          int index = primitiveIndices[node->primitiveIndicesOffset + i];
                          const std::shared_ptr<Object> &p = primitives[index];
                          //<<Check one primitive inside leaf node>> 
                             double u, v, t;
                             ray->intersections++;
                             if (p->intersected(ray, index, u, v, t)) 
                                 hit = true;
                      }
                  }

               //<<Grab next node to process from todo list>> 
                  if (todoPos > 0) {
                      --todoPos;
                      node = todo[todoPos].node;
                      tMin = todo[todoPos].tMin;
                      tMax = todo[todoPos].tMax;
                  }
                  else
                      break;
           }
       }
    return hit;
}