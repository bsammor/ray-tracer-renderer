#include <kdtree.h>

static unsigned int log2_int(unsigned int val) 
{
    if (val == 0) return UINT_MAX;
    if (val == 1) return 0;
    unsigned int ret = 0;
    while (val > 1) 
    {
        val >>= 1;
        ret++;
    }
    return ret;
}

KDtree::KDtree(const std::vector<std::shared_ptr<Object>> &prims,int isect_cost, int traversal_cost, double empty_bonus,int max_prims, int max_depth)
    : isect_cost(isect_cost), traversal_cost(traversal_cost), max_prims(max_prims), empty_bonus(empty_bonus), primitives(prims) 
{
    next_free_node = nodes_count = 0;
    if (max_depth <= 0) max_depth = std::round(8 + 1.3f * log2_int(primitives.size()));

    std::vector<BBOX> prim_bounds;
    for (const std::shared_ptr<Object> &prim : primitives) 
    {
        BBOX b = prim->get_bbox();
        bounds = BBOX::union_bbox(bounds, b);
        prim_bounds.push_back(b);
    }

    std::unique_ptr<bound_edge[]> edges[3];
    for (int i = 0; i < 3; ++i)
        edges[i].reset(new bound_edge[2 * primitives.size()]);
    std::unique_ptr<int[]> prims0(new int[primitives.size()]);
    std::unique_ptr<int[]> prims1(new int[(max_depth+1) * primitives.size()]);

    std::unique_ptr<int[]> prim_nums(new int[primitives.size()]);
    for (size_t i = 0; i < primitives.size(); ++i)
        prim_nums[i] = i;

    
    build_tree(0, bounds, prim_bounds, prim_nums.get(), primitives.size(), 
            max_depth, edges, prims0.get(), prims1.get());
}


void KDtree::build_tree(int node_index, const BBOX &node_bounds, const std::vector<BBOX> &allprim_bounds, int *prim_nums, int prims_count, int depth,
        const std::unique_ptr<bound_edge[]> edges[3], int *prims0, int *prims1, int bad_refines) 
{
    if (next_free_node == nodes_count) 
    {
        int new_nodes_count = std::max(2 * nodes_count, 512);
        KD_node *n = new KD_node[new_nodes_count];
        if (nodes_count > 0) 
        {
            memcpy(n, nodes, nodes_count * sizeof(KD_node));
            free(nodes);
        }
        nodes = n;
        nodes_count = new_nodes_count;
    }
    ++next_free_node;

    if (prims_count <= max_prims || depth == 0) 
    {
        nodes[node_index].init_leaf(prim_nums, prims_count, &prims_indices);
        return;
    }

    int best_axis = -1, best_offset = -1;
    double best_cost = INFINITY;
    double old_cost = isect_cost * double(prims_count);
    double total_SA = node_bounds.surface_area();
    double inv_total_SA = 1 / total_SA;
    Vec3 d = node_bounds.max - node_bounds.min;

    int axis = node_bounds.maximum_extent();
    int retries = 0;
    retry_split:
        for (int i = 0; i < prims_count; ++i) 
        {
            int pn = prim_nums[i];
            const BBOX &bounds = allprim_bounds[pn];
            edges[axis][2 * i] =     bound_edge(bounds.min[axis], pn, true);
            edges[axis][2 * i + 1] = bound_edge(bounds.max[axis], pn, false);
        }

        std::sort(&edges[axis][0], &edges[axis][2*prims_count], [](const bound_edge &e0, const bound_edge &e1) -> bool 
        {
            if (e0.t == e1.t) return (int)e0.type < (int)e1.type;
            else return e0.t < e1.t; 
        });

        int num_below = 0, num_above = prims_count;
        for (int i = 0; i < 2 * prims_count; ++i) 
        {
            if (edges[axis][i].type == edge_type::end) --num_above;
            double edgeT = edges[axis][i].t;
            if (edgeT > node_bounds.min[axis] && edgeT < node_bounds.max[axis]) 
            {
                int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
                double below_SA = 2 * (d[otherAxis0] * d[otherAxis1] + (edgeT - node_bounds.min[axis]) * (d[otherAxis0] + d[otherAxis1]));
                double above_SA = 2 * (d[otherAxis0] * d[otherAxis1] + (node_bounds.max[axis] - edgeT) * (d[otherAxis0] + d[otherAxis1]));

                double p_below = below_SA * inv_total_SA; 
                double p_above = above_SA * inv_total_SA;
                double b_e = (num_above == 0 || num_below == 0) ? empty_bonus : 0;
                double cost = traversal_cost + isect_cost * (1 - b_e) * (p_below * num_below + p_above * num_above);
                if (cost < best_cost)  
                {
                    best_cost = cost;
                    best_axis = axis;
                    best_offset = i;
                }
            }
            if (edges[axis][i].type == edge_type::start) ++num_below;
        }
             

        if (best_axis == -1 && retries < 2) 
        {
            ++retries;
            axis = (axis + 1) % 3;
            goto retry_split;
        }
        if (best_cost > old_cost) ++bad_refines;
        if ((best_cost > 4 * old_cost && prims_count < 16) || best_axis == -1 || bad_refines == 3) 
        {
            nodes[node_index].init_leaf(prim_nums, prims_count, &prims_indices);
            return; 
        }

        int n0 = 0, n1 = 0;
        for (int i = 0; i < best_offset; ++i)
            if (edges[best_axis][i].type == edge_type::start)
                prims0[n0++] = edges[best_axis][i].prim_index;
        for (int i = best_offset + 1; i < 2 * prims_count; ++i)
            if (edges[best_axis][i].type == edge_type::end)
                prims1[n1++] = edges[best_axis][i].prim_index;

        double t_split = edges[best_axis][best_offset].t;
        BBOX bounds0 = node_bounds, bounds1 = node_bounds;
        bounds0.max[best_axis] = bounds1.min[best_axis] = t_split;
        build_tree(node_index + 1, bounds0, allprim_bounds, prims0, n0,
                depth - 1, edges, prims0, prims1 + prims_count, bad_refines);
        int aboveChild = next_free_node;
        nodes[node_index].init_interior(best_axis, aboveChild, t_split);
        build_tree(aboveChild, bounds1, allprim_bounds, prims1, n1, 
                depth - 1, edges, prims0, prims1 + prims_count, bad_refines);
}

bool KDtree::intersect_tree(std::shared_ptr<Ray> ray) 
{
    double tmin, tmax;
    if (!bounds.intersected(ray, &tmin, &tmax)) return false;
       
    Vec3 inv_dir(1 / ray->get_direction().x, 1 / ray->get_direction().y, 1 / ray->get_direction().z);
    constexpr int max_todo = 64;
    KD_todo todo[max_todo];
    int todo_pos = 0;
    bool intersected = false;
    const KD_node *node = &nodes[0];

    while (node != nullptr) 
    {
        if (ray->tmax < tmin) break;

        if (!node->is_leaf()) 
        {
            int axis = node->get_split_axis();
            double tPlane = (node->get_split_pos() - ray->get_origin()[axis]) * inv_dir[axis];

            const KD_node *first_child, *second_child;
            int below_first = (ray->get_origin()[axis] <  node->get_split_pos()) || (ray->get_origin()[axis] == node->get_split_pos() && ray->get_direction()[axis] <= 0);
            
            if (below_first) 
            {
                first_child = node + 1;
                second_child = &nodes[node->get_above_child()];
            } 
            else 
            {
                first_child = &nodes[node->get_above_child()];
                second_child = node + 1;
            }

            if (tPlane > tmax || tPlane <= 0) node = first_child;
            else if (tPlane < tmin) node = second_child;
            else 
            {
                todo[todo_pos].node = second_child;
                todo[todo_pos].tmin = tPlane;
                todo[todo_pos].tmax = tmax;
                ++todo_pos;

                node = first_child;
                tmax = tPlane;
            }

        } 
        else 
        {
            int prims_count = node->get_prims_count();

            if (prims_count == 1) 
            {
                const std::shared_ptr<Object> &p = primitives[node->one_primitive];
                if (p->intersected(ray, node->one_primitive)) intersected = true;
            } 
            else 
            {
                for (int i = 0; i < prims_count; ++i) 
                {
                    int index = prims_indices[node->prim_indices_offset + i];
                    const std::shared_ptr<Object> &p = primitives[index];
                    if (p->intersected(ray, index)) intersected = true;
                }
            }

            if (todo_pos > 0) 
            {
                --todo_pos;
                node = todo[todo_pos].node;
                tmin = todo[todo_pos].tmin;
                tmax = todo[todo_pos].tmax;
            }
            else break;
        }
    }

    return intersected;
}