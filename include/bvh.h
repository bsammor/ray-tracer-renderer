#ifndef BVH_H
#define BVH_H
#include <bbox.h>
#include <vector>
#include <triangle.h>
#include <atomic>
#include "memory.h"

struct linear_BVH_node {
    BBOX bounds;
    union {
        int primitivesOffset;    // leaf
        int secondChildOffset;   // interior
    };
    uint16_t nPrimitives;  // 0 -> interior node
    uint8_t axis;          // interior node: xyz
    uint8_t pad[1];        // ensure 32 byte total size
};

struct BVH_primitive_info
{
    BVH_primitive_info(size_t primitive_id, BBOX bounds)
        : primitive_id(primitive_id), bounds(bounds),
          center(bounds.get_center()) { }

    size_t primitive_id;
    BBOX bounds;
    Vec3 center;
};

struct BVH_node 
{
    void InitLeaf(int first, int n, const BBOX &b) {
        first_prim_id = first;
        prim_num = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }
    void InitInterior(int axis, BVH_node *c0, BVH_node *c1) {
        children[0] = c0;
        children[1] = c1;
        bounds = BBOX::union_bbox(c0->bounds, c1->bounds);
        splitAxis = axis;
        prim_num = 0;
    }
    BBOX bounds;
    BVH_node *children[2];
    int splitAxis, first_prim_id, prim_num;
};

BVH_node* build_tree(std::vector<BVH_primitive_info>  primitive_info, int start,
        int end, int *totalNodes, std::vector<std::shared_ptr<Object>> &orderedPrims, std::vector<std::shared_ptr<Object>> primitives, int max_prim_num) 
{
    BVH_node *node = new BVH_node;
    (*totalNodes)++;
    BBOX bounds;
    for (int i = start; i < end; ++i)
           bounds = BBOX::union_bbox(bounds, primitive_info[i].bounds);

    int nPrimitives = end - start;
    if (nPrimitives == 1) 
    {
        int firstPrimOffset = orderedPrims.size();
        for (int i = start; i < end; ++i) {
            int primNum = primitive_info[i].primitive_id;
            orderedPrims.push_back(primitives[primNum]);
        }
        node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
        return node;
    }
    else
    {
        BBOX centroidBounds;
        for (int i = start; i < end; ++i)
            centroidBounds = BBOX::union_bbox(centroidBounds, primitive_info[i].center);
        int dim = centroidBounds.maximum_extent();

        int mid = (start + end) / 2;
        if (centroidBounds.max[dim] == centroidBounds.min[dim]) {
            int firstPrimOffset = orderedPrims.size();
            for (int i = start; i < end; ++i) {
                int primNum = primitive_info[i].primitive_id;
                orderedPrims.push_back(primitives[primNum]);
            }
            node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
            return node;
        } 
        else 
        {
            int split_method = 1;
            switch(split_method) 
            {
                case 0:
                {
                    double pmid = (centroidBounds.min[dim] + centroidBounds.max[dim]) / 2;
                                BVH_primitive_info* midPtr =
                                    std::partition(&primitive_info[start], &primitive_info[end-1]+1,
                                        [dim, pmid](const BVH_primitive_info &pi) {
                                            return pi.center[dim] < pmid;
                                        });
                                mid = midPtr - &primitive_info[0];
                                if (mid != start && mid != end)
                                    break;
                }
                case 1:
                {
                    if (nPrimitives <= 4) 
                    {
                        mid = (start + end) / 2;
                        std::nth_element(&primitive_info[start], &primitive_info[mid], 
                                        &primitive_info[end-1]+1,
                            [dim](const BVH_primitive_info &a, const BVH_primitive_info &b) { 
                                return a.center[dim] < b.center[dim];
                            });

                    } else {
                            constexpr int nBuckets = 12;
                            struct BucketInfo 
                            {
                                int count = 0;
                                BBOX bounds;
                            };
                            BucketInfo buckets[nBuckets]; 
                            for (int i = start; i < end; ++i) 
                            {
                                int b = nBuckets * 
                                    centroidBounds.offset(primitive_info[i].center)[dim];
                                if (b == nBuckets) b = nBuckets - 1;
                                buckets[b].count++;
                                buckets[b].bounds = BBOX::union_bbox(buckets[b].bounds, primitive_info[i].bounds);
                            }
                            double cost[nBuckets - 1];
                            for (int i = 0; i < nBuckets - 1; ++i) 
                            {
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
                            double minCost = cost[0];
                            int minCostSplitBucket = 0;
                            for (int i = 1; i < nBuckets - 1; ++i) 
                            {
                                if (cost[i] < minCost) {
                                    minCost = cost[i];
                                    minCostSplitBucket = i;
                                }
                            }
                            double leafCost = nPrimitives;
                            if (nPrimitives > max_prim_num || minCost < leafCost) {
                                BVH_primitive_info *pmid = std::partition(&primitive_info[start],
                                    &primitive_info[end-1]+1, 
                                    [=](BVH_primitive_info &pi) {
                                        int b = nBuckets * centroidBounds.offset(pi.center)[dim];
                                        if (b == nBuckets) b = nBuckets - 1;
                                        return b <= minCostSplitBucket;
                                    });
                                mid = pmid - &primitive_info[0];
                            } else {
                                int firstPrimOffset = orderedPrims.size();
                                for (int i = start; i < end; ++i) {
                                    int primNum = primitive_info[i].primitive_id;
                                    orderedPrims.push_back(primitives[primNum]);
                                }
                                node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
                                return node;
                            }
                    }

                    break;
                }
            }
            node->InitInterior(dim,
                                build_tree(primitive_info, start, mid,
                                                totalNodes, orderedPrims, primitives, max_prim_num),
                                build_tree(primitive_info, mid, end,
                                                totalNodes, orderedPrims, primitives, max_prim_num));
        }
    }
    
    return node;
}

int flatten_tree(BVH_node *node, int *offset, linear_BVH_node *nodes) 
{
    linear_BVH_node *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    int myOffset = (*offset)++;
    if (node->prim_num > 0) {
        linearNode->primitivesOffset = node->first_prim_id;
        linearNode->nPrimitives = node->prim_num;
    } else {
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;
        flatten_tree(node->children[0], offset, nodes);
        linearNode->secondChildOffset =
        flatten_tree(node->children[1], offset, nodes);
    }
    return myOffset;
}

struct BVH 
{
public:
    int max_prim_num;
    BVH_node *root;
    linear_BVH_node *nodes = nullptr;
    std::vector<std::shared_ptr<Object>> primitives;
    BVH(std::vector<std::shared_ptr<Object>> primitives, int max_prim_num) : max_prim_num(std::min(255, max_prim_num)), primitives(primitives) 
    {
        std::vector<BVH_primitive_info> primitive_info;
        for (size_t i = 0; i < primitives.size(); ++i) 
        {
            primitive_info.push_back({i, primitives[i]->get_bbox()});
        }

        int totalNodes = 0;
        std::vector<std::shared_ptr<Object>> ordered_prim;
        ordered_prim.reserve(primitives.size());
        root = build_tree(primitive_info, 0, primitives.size(),
                          &totalNodes, ordered_prim, primitives, max_prim_num);
        primitives.swap(ordered_prim);

        nodes = new linear_BVH_node[totalNodes];
        int offset = 0;
        flatten_tree(root, &offset, nodes);

        //std::cout << "done flattening " << totalNodes << " nodes" <<
    }
};

bool intersected_BVH(std::shared_ptr<Ray> camera_ray, std::shared_ptr<BVH> tree) 
{
bool hit = false;
		camera_ray->invdir = Vec3(1 / camera_ray->get_direction().x, 1 / camera_ray->get_direction().y, 1 / camera_ray->get_direction().z); 
        camera_ray->sign[0] = (camera_ray->invdir.x < 0); 
        camera_ray->sign[1] = (camera_ray->invdir.y < 0); 
        camera_ray->sign[2] = (camera_ray->invdir.z < 0); 
		int toVisitOffset = 0, currentNodeIndex = 0;
		int nodesToVisit[12000];
		while (true) {
			linear_BVH_node *node = &(tree->nodes)[currentNodeIndex];
			if (node->bounds.IntersectP(camera_ray, camera_ray->invdir, camera_ray->sign)) {
				if (node->nPrimitives > 0) {
					double u ,v ,t;
					for (int i = 0; i < node->nPrimitives; ++i)
						if (tree->primitives[node->primitivesOffset + i]->intersected(camera_ray, node->primitivesOffset + i, u, v, t)) {
							camera_ray->u = u;
							camera_ray->v = v;
							camera_ray->set_tmax(t);
							camera_ray->set_index(node->primitivesOffset + i);
							hit = true;
						}
					if (toVisitOffset == 0) break;
					currentNodeIndex = nodesToVisit[--toVisitOffset];

				} else {
					if (camera_ray->sign[node->axis]) {
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

#endif