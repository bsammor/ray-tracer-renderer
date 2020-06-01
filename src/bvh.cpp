#include <bvh.h>

BBOX BVHAccel::WorldBound() const {
    return nodes ? nodes[0].bounds : BBOX();
}

BVHAccel::BVHAccel(std::vector<std::shared_ptr<Object>> &p,
         int maxPrimsInNode)
     : maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(p)
{
    if (primitives.size() == 0)
        return;
    std::vector<BVHPrimitiveInfo> primitiveInfo;
    for (size_t i = 0; i < primitives.size(); ++i) {
        BBOX temp = primitives[i]->get_bbox();
        primitiveInfo.push_back(BVHPrimitiveInfo(i, temp));
    }
    int totalNodes = 0;
    std::vector<std::shared_ptr<Object>> orderedPrims;
    BVHBuildNode *root;
    root = recursiveBuild(primitiveInfo, 0, primitives.size(),
                            &totalNodes, orderedPrims);
    primitives.swap(orderedPrims);
    
    nodes = new LinearBVHNode[totalNodes];
    int offset = 0;
    flattenBVHTree(root, &offset);
}


BVHBuildNode *BVHAccel::recursiveBuild(std::vector<BVHPrimitiveInfo> &primitiveInfo, int start,
        int end, int *totalNodes,
        std::vector<std::shared_ptr<Object>> &orderedPrims) {
    BVHBuildNode *node = new BVHBuildNode();
    (*totalNodes)++;
    //<<Compute bounds of all primitives in BVH node>> 
       BBOX bounds;
       for (int i = start; i < end; ++i)
           bounds = BBOX::union_bbox(bounds, primitiveInfo[i].bounds);

    int nPrimitives = end - start;
    if (nPrimitives == 1) {
        //<<Create leaf BVHBuildNode>> 
           int firstPrimOffset = orderedPrims.size();
           for (int i = start; i < end; ++i) {
               int primNum = primitiveInfo[i].primitiveNumber;
               orderedPrims.push_back(primitives[primNum]);
           }
           node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
           return node;

    } else {
        //<<Compute bound of primitive centroids, choose split dimension dim>> 
           double minNum = std::numeric_limits<double>::lowest();
           double maxNum = std::numeric_limits<double>::max();
           Vec3 pMin = Vec3(maxNum, maxNum, maxNum);
           Vec3 pMax = Vec3(minNum, minNum, minNum);
           BBOX centroidBounds(pMin, pMax);
           for (int i = start; i < end; ++i)
               centroidBounds = BBOX::union_bbox(centroidBounds, primitiveInfo[i].centroid);
           int dim = centroidBounds.MaximumExtent();

        //<<Partition primitives into two sets and build children>> 
           int mid = (start + end) / 2;
           if (centroidBounds.max[dim] == centroidBounds.min[dim]) {
               //<<Create leaf BVHBuildNode>> 
                  int firstPrimOffset = orderedPrims.size();
                  for (int i = start; i < end; ++i) {
                      int primNum = primitiveInfo[i].primitiveNumber;
                      orderedPrims.push_back(primitives[primNum]);
                  }
                  node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
                  return node;

           } else {
               //<<Partition primitives based on splitMethod>> 
                      //<<Partition primitives using approximate SAH>> 
                         if (nPrimitives <= 4) {
                             //<<Partition primitives into equally sized subsets>> 
                                mid = (start + end) / 2;
                                std::nth_element(&primitiveInfo[start], &primitiveInfo[mid], 
                                                 &primitiveInfo[end-1]+1,
                                    [dim](const BVHPrimitiveInfo &a, const BVHPrimitiveInfo &b) { 
                                        return a.centroid[dim] < b.centroid[dim];
                                    });

                         } else {
                             //<<Allocate BucketInfo for SAH partition buckets>> 
                                constexpr int nBuckets = 12;
                                struct BucketInfo {
                                    int count = 0;
                                    BBOX bounds;
                                };
                                BucketInfo buckets[nBuckets];

                             //<<Initialize BucketInfo for SAH partition buckets>> 
                                for (int i = start; i < end; ++i) {
                                    int b = nBuckets * 
                                        centroidBounds.offset(primitiveInfo[i].centroid)[dim];
                                    if (b == nBuckets) b = nBuckets - 1;
                                    buckets[b].count++;
                                    buckets[b].bounds = BBOX::union_bbox(buckets[b].bounds, primitiveInfo[i].bounds);
                                }

                             //<<Compute costs for splitting after each bucket>> 
                                double cost[nBuckets - 1];
                                for (int i = 0; i < nBuckets - 1; ++i) {
                                    BBOX b0, b1;
                                    int count0 = 0, count1 = 0;
                                    for (int j = 0; j <= i; ++j) {
                                        b0 = BBOX::union_bbox(b0, buckets[j].bounds);
                                        count0 += buckets[j].count;
                                    }
                                    for (int j = i+1; j < nBuckets; ++j) {
                                        b1 = BBOX::union_bbox(b1, buckets[j].bounds);
                                        count1 += buckets[j].count;
                                    }
                                    cost[i] = .125f + (count0 * b0.surface_area() +
                                                       count1 * b1.surface_area()) / bounds.surface_area();
                                }

                             //<<Find bucket to split at that minimizes SAH metric>> 
                                double minCost = cost[0];
                                int minCostSplitBucket = 0;
                                for (int i = 1; i < nBuckets - 1; ++i) {
                                    if (cost[i] < minCost) {
                                        minCost = cost[i];
                                        minCostSplitBucket = i;
                                    }
                                }

                             //<<Either create leaf or split primitives at selected SAH bucket>> 
                                double leafCost = nPrimitives;
                                if (nPrimitives > maxPrimsInNode || minCost < leafCost) {
                                    BVHPrimitiveInfo *pmid = std::partition(&primitiveInfo[start],
                                        &primitiveInfo[end-1]+1, 
                                        [=](BVHPrimitiveInfo &pi) {
                                            int b = nBuckets * centroidBounds.offset(pi.centroid)[dim];
                                            if (b == nBuckets) b = nBuckets - 1;
                                            return b <= minCostSplitBucket;
                                        });
                                    mid = pmid - &primitiveInfo[0];
                                } else {
                                    //<<Create leaf BVHBuildNode>> 
                                       int firstPrimOffset = orderedPrims.size();
                                       for (int i = start; i < end; ++i) {
                                           int primNum = primitiveInfo[i].primitiveNumber;
                                           orderedPrims.push_back(primitives[primNum]);
                                       }
                                       node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
                                       return node;

                                }

                         }

               node->InitInterior(dim,
                                  recursiveBuild(primitiveInfo, start, mid,
                                                 totalNodes, orderedPrims),
                                  recursiveBuild(primitiveInfo, mid, end,
                                                 totalNodes, orderedPrims));
           }

    }
    return node;
}

int BVHAccel::flattenBVHTree(BVHBuildNode *node, int *offset) {
    LinearBVHNode *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    int myOffset = (*offset)++;
    if (node->nPrimitives > 0) {
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    } else {
           linearNode->axis = node->splitAxis;
           linearNode->nPrimitives = 0;
           flattenBVHTree(node->children[0], offset);
           linearNode->secondChildOffset =
               flattenBVHTree(node->children[1], offset);

    }
    return myOffset;
}

bool BVHAccel::intersect_tree(std::shared_ptr<Ray> ray) const {
    bool hit = false;
    Vec3 invDir(1 / ray->get_direction().x, 1 / ray->get_direction().y, 1 / ray->get_direction().z);
    int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
       int toVisitOffset = 0, currentNodeIndex = 0;
       int nodesToVisit[64];
       while (true) {
           const LinearBVHNode *node = &nodes[currentNodeIndex];
              if (node->bounds.IntersectP(ray, invDir, dirIsNeg)) {
                  if (node->nPrimitives > 0) {
                         for (int i = 0; i < node->nPrimitives; ++i) 
                            {
                                double u, v, t;
                                ray->intersections++;
                                if (primitives[node->primitivesOffset + i]->intersected(ray, node->primitivesOffset + i, u, v, t))
                                 hit = true;
                            }
                         if (toVisitOffset == 0) break;
                         currentNodeIndex = nodesToVisit[--toVisitOffset];

                  } else {
                         if (dirIsNeg[node->axis]) {
                            nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                            currentNodeIndex = node->secondChildOffset;
                         } else {
                            nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                            currentNodeIndex = currentNodeIndex + 1;
                         }

                  }
              } else {
                  if (toVisitOffset == 0) break;
                  currentNodeIndex = nodesToVisit[--toVisitOffset];
              }

       }

    return hit;
}