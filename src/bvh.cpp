#include <bvh.h>

BVH::BVH(std::vector<std::shared_ptr<Object>> &prims, int min_prims) : min_prims(std::min(min_prims, 255)), primitives(prims)
{
    if (primitives.size() == 0) return;

    std::vector<prim_info> prims_info;
    for (size_t i = 0; i < primitives.size(); ++i) 
    {
        BBOX temp = primitives[i]->get_bbox();
        prims_info.push_back(prim_info(i, temp));
    }

    int total_nodes = 0;
    std::vector<std::shared_ptr<Object>> ordered_prims;
    BVH_node *root = build_hierarchy(prims_info, 0, primitives.size(), &total_nodes, ordered_prims);

    primitives.swap(ordered_prims);
    
    nodes = new linear_BVH_node[total_nodes];
    int offset = 0;
    flatten_hierarchy(root, &offset);
}

BVH_node* BVH::build_hierarchy(std::vector<prim_info> &prims_info, int start, int end, int *total_nodes, std::vector<std::shared_ptr<Object>> &ordered_prims)
{
    BVH_node *node = new BVH_node();
    (*total_nodes)++;

    BBOX bounds;
    for (int i = start; i < end; ++i)
        bounds = BBOX::union_bbox(bounds, prims_info[i].bounds);

    int prims_count = end - start;

    if (prims_count == 1) 
    {
        int first_prim_offset = ordered_prims.size();
        for (int i = start; i < end; ++i) 
        {
            int prim_index = prims_info[i].prim_index;
            ordered_prims.push_back(primitives[prim_index]);
        }
        node->init_leaf(first_prim_offset, prims_count, bounds);

        return node;
    } 
    else 
    {
        Vec3 min = Vec3(max_val, max_val, max_val);
        Vec3 max = Vec3(min_val, min_val, min_val);
        BBOX center_bounds(min, max);

        for (int i = start; i < end; ++i)
            center_bounds = BBOX::union_bbox(center_bounds, prims_info[i].center);
        int dim = center_bounds.maximum_extent();

        int mid = (start + end) / 2;

        if (center_bounds.max[dim] == center_bounds.min[dim]) 
        {
            int first_prim_offset = ordered_prims.size();
            for (int i = start; i < end; ++i) 
            {
                int prim_index = prims_info[i].prim_index;
                ordered_prims.push_back(primitives[prim_index]);
            }
            node->init_leaf(first_prim_offset, prims_count, bounds);

            return node;
        } 
        else 
        {
            if (prims_count <= 4) 
            {
                mid = (start + end) / 2;
                std::nth_element(&prims_info[start], &prims_info[mid], &prims_info[end-1]+1, [dim](const prim_info &a, const prim_info &b) 
                { return a.center[dim] < b.center[dim];});
            } 
            else 
            {
                // create buckets
                constexpr int bucket_count = 12;
                bucket_info buckets[bucket_count];

                // calculate buckets information
                for (int i = start; i < end; ++i) 
                {
                    int b = bucket_count * center_bounds.offset(prims_info[i].center)[dim];
                    if (b == bucket_count) b = bucket_count - 1;
                    buckets[b].count++;
                    buckets[b].bounds = BBOX::union_bbox(buckets[b].bounds, prims_info[i].bounds);
                }

                double cost[bucket_count - 1];

                // calculate cost of each bucket
                for (int i = 0; i < bucket_count - 1; ++i) 
                {
                    BBOX b0, b1;
                    int count0 = 0, count1 = 0;
                    for (int j = 0; j <= i; ++j) 
                    {
                        b0 = BBOX::union_bbox(b0, buckets[j].bounds);
                        count0 += buckets[j].count;
                    }
                    for (int j = i+1; j < bucket_count; ++j) 
                    {
                        b1 = BBOX::union_bbox(b1, buckets[j].bounds);
                        count1 += buckets[j].count;
                    }
                    // SAH
                    cost[i] = 0.125 + (count0 * b0.surface_area() +
                                        count1 * b1.surface_area()) / bounds.surface_area();
                }

                // choose least cost bucket
                double min_cost = cost[0];
                int min_bucket_index = 0;
                for (int i = 1; i < bucket_count - 1; ++i) 
                {
                    if (cost[i] < min_cost) 
                    {
                        min_cost = cost[i];
                        min_bucket_index = i;
                    }
                }

                double leaf_cost = prims_count;
                if (prims_count > min_prims || min_cost < leaf_cost) 
                {
                    prim_info *pmid = std::partition(&prims_info[start], &prims_info[end-1]+1, [=](prim_info &pi) 
                    {
                        int b = bucket_count * center_bounds.offset(pi.center)[dim];
                        if (b == bucket_count) b = bucket_count - 1;

                        return b <= min_bucket_index;
                    });
                    mid = pmid - &prims_info[0];
                } 
                else 
                {
                    int first_prim_offset = ordered_prims.size();
                    for (int i = start; i < end; ++i) {
                        int prim_index = prims_info[i].prim_index;
                        ordered_prims.push_back(primitives[prim_index]);
                    }
                    node->init_leaf(first_prim_offset, prims_count, bounds);

                    return node;
                }
            }

            node->init_interior(dim, build_hierarchy(prims_info, start, mid, total_nodes, ordered_prims), 
                build_hierarchy(prims_info, mid, end, total_nodes, ordered_prims));
        }
    }
    return node;
}

int BVH::flatten_hierarchy(BVH_node *node, int *offset)
{
    linear_BVH_node *linear_node = &nodes[*offset];
    linear_node->bounds = node->bounds;
    int my_offset = (*offset)++;

    if (node->prims_count > 0) 
    {
        linear_node->prims_offset = node->first_prim_offset;
        linear_node->prims_count = node->prims_count;
    } 
    else 
    {
        linear_node->axis = node->axis;
        linear_node->prims_count = 0;
        flatten_hierarchy(node->children[0], offset);
        linear_node->second_child_offset = flatten_hierarchy(node->children[1], offset);
    }

    return my_offset;
}

bool BVH::intersect_tree(std::shared_ptr<Ray> ray)
{
    bool intersected = false;
    
    Vec3 inv_dir(1 / ray->get_direction().x, 1 / ray->get_direction().y, 1 / ray->get_direction().z);
    int dir_is_neg[3] = { inv_dir.x < 0, inv_dir.y < 0, inv_dir.z < 0 };
    int visit_offset = 0, node_index = 0;
    int nodes_left[64];
    
    while (true) 
    {
        const linear_BVH_node *node = &nodes[node_index];
        if (node->bounds.intersected(ray, inv_dir, dir_is_neg)) 
        {
            if (node->prims_count > 0) 
            {
                for (int i = 0; i < node->prims_count; ++i) 
                {
                    if (primitives[node->prims_offset + i]->intersected(ray, node->prims_offset + i))
                        intersected = true;
                }
                if (visit_offset == 0) break;
                node_index = nodes_left[--visit_offset];
            } 
            else 
            {
                if (dir_is_neg[node->axis]) 
                {
                    nodes_left[visit_offset++] = node_index + 1;
                    node_index = node->second_child_offset;
                } 
                else 
                {
                    nodes_left[visit_offset++] = node->second_child_offset;
                    node_index = node_index + 1;
                }
            }
        } 
        else 
        {
            if (visit_offset == 0) break;
            node_index = nodes_left[--visit_offset];
        }
    }

    return intersected;
}