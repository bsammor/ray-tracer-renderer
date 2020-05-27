#include <tree.h>
#include <ray.h>
#include <memory>
#include <bbox.h>
#include <iostream>
#include <vector>
#include <object.h>
#include <stdio.h>
#include <algorithm> 

struct SplitPlane {
    SplitPlane(const int axis, const float pos): axis(axis), pos(pos){};
    SplitPlane(){};

    int axis; // 0=x, 1=y, 2=z;
    float pos;

    bool operator==(const SplitPlane& sp) {
        return(axis == sp.axis && pos == sp.pos);
    }
};


class Node : public Tree
{
public:
    Node* left;
    Node* right;
    bool is_leaf;
    SplitPlane splitPlane;
    BBOX bounds;
    std::vector<std::shared_ptr<Object>> objects;
    bool intersect_tree(std::shared_ptr<Ray> ray) const;

    Node():is_leaf(false) {};
    bool isDone(int N, float minCv) const;
    float calculateCost(const float &position);
    void traverse(std::shared_ptr<Ray> r);
    void traverse(std::shared_ptr<Ray> r, float t_near, float t_far);
    void splitBox(const BBOX& V, const SplitPlane& p, BBOX& VL, BBOX& VR) const;
    typedef enum { LEFT=-1, RIGHT=1, UNKNOWN=0 } PlaneSide;
    void SAH(const SplitPlane& p, const BBOX& V, int NL, int NR, int NP, float& CP, PlaneSide& pside) const;
    void findPlane(std::vector<std::shared_ptr<Object>> T, const BBOX& V, int depth, SplitPlane& p_est, float& C_est, PlaneSide& pside_est) const;
    void sortTriangles(std::vector<std::shared_ptr<Object>> T, const SplitPlane& p, const PlaneSide& pside, std::vector<std::shared_ptr<Object>> TL, std::vector<std::shared_ptr<Object>> TR) const;
    Node* RecBuild(std::vector<std::shared_ptr<Object>> T, const BBOX &V, int depth, const SplitPlane& prev_plane);
};