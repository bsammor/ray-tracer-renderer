#include <kdtree1.h>

void Node::traverse(std::shared_ptr<Ray> r){
    return traverse(r, bounds.tmin, bounds.tmax);
}

void Node::traverse(std::shared_ptr<Ray> ray, float t_min, float t_max)
{
    if (is_leaf) {
        for (auto object: objects) {
            double u, v, t;
            object->intersected(ray, 0, u, v, t);
        }
    }
    else{
        //float t_split = distanceAlongRayToPlane(ray);
        Vec3 inv_direction = Vec3(1.0/ray->get_direction().x, 1.0/ray->get_direction().y, 1.0/ray->get_direction().z);
        float t_split = (splitPlane.pos - ray->get_origin()[splitPlane.axis]) * (ray->get_direction()[splitPlane.axis] == 0 ? INFINITY : inv_direction[splitPlane.axis]);

        // near is the side containing the origin of the ray
        Node *near, *far;
        if(ray->get_origin()[splitPlane.axis] < splitPlane.pos) {
            near = left;
            far = right;
        } else {
            near = right;
            far = left;
        }

        if( t_split > t_max || t_split < 0) {
            near->traverse(ray, t_min, t_max);
        }
        else if(t_split < t_min) {
            far->traverse(ray, t_min, t_max);
        }
        else {
            near->traverse(ray, t_min, t_split);
            if(ray->get_tmax() < t_split)
                return;
            far->traverse(ray, t_split, t_max);
        }
    }
}


#define COST_TRAVERSE 1.0
#define COST_INTERSECT 1.5

void Node::splitBox(const BBOX& V, const SplitPlane& p, BBOX& VL, BBOX& VR) const {
    VL = V;
    VR = V;
    VL.max[p.axis] = p.pos;
    VR.min[p.axis] = p.pos;
}

// surface area of a volume V
inline float surfaceArea(const BBOX& V) {
    return 2*V.dx()*V.dy() + 2*V.dx()*V.dz() + 2*V.dy()*V.dz();
}

// Probability of hitting volume Vsub, given volume V was hit
float prob_hit(const BBOX& Vsub, const BBOX& V){
    return surfaceArea(Vsub) / surfaceArea(V);
}

// bias for the cost function s.t. it is reduced if NL or NR becomes zero
inline float lambda(int NL, int NR, float PL, float PR) {
    if((NL == 0 || NR == 0) &&
       !(PL == 1 || PR == 1) // NOT IN PAPER
       )
        return 0.8f;
    return 1.0f;
}

inline float cost(float PL, float PR, int NL, int NR) {
    return(lambda(NL, NR, PL, PR) * (COST_TRAVERSE + COST_INTERSECT * (PL * NL + PR * NR)));
}

// SAH heuristic for computing the cost of splitting a voxel V using a plane p
void Node::SAH(const SplitPlane& p, const BBOX& V, int NL, int NR, int NP, float& CP, PlaneSide& pside) const {
    CP = INFINITY;
    BBOX VL, VR;
    splitBox(V, p, VL, VR);
    float PL, PR;
    PL = prob_hit(VL, V);
    PR = prob_hit(VR, V);
    if(PL == 0 || PR == 0) // NOT IN PAPER
        return;
    if(V.d(p.axis) == 0) // NOT IN PAPER
        return;
    float CPL, CPR;
    CPL = cost(PL, PR, NL + NP, NR);
    CPR = cost(PL, PR, NL, NP + NR );
    if(CPL < CPR) {
        CP = CPL;
        pside = LEFT;
    } else {
        CP = CPR;
        pside = RIGHT;
    }
}

// criterion for stopping subdividing a tree node
inline bool Node::isDone(int N, float minCv) const {
    // cerr << "terminate: minCv=" << minCv << ", KI*N=" << KI*N << endl;
    return(minCv > COST_INTERSECT*N);
}


// get primitives's clipped bounding box
BBOX clipTriangleToBox(std::shared_ptr<Object> t, const BBOX& V) {
    BBOX b = t->get_bbox();
    for(int k=0; k<3; k++) {
        if(V.min[k] > b.min[k])
            b.min[k] = V.min[k];
        if(V.max[k] < b.max[k])
            b.max[k] = V.max[k];
    }
    return b;
}

struct Event {
    typedef enum { endingOnPlane=0, lyingOnPlane=1, startingOnPlane=2  } EventType;
    std::shared_ptr<Object> triangle;
    SplitPlane splitPlane;
    EventType type;

    Event(std::shared_ptr<Object> tri, int k, float ee0, EventType type) : triangle(tri), type(type) , splitPlane(SplitPlane(k, ee0)){}

    inline bool operator<(const Event& e) const {
        return((splitPlane.pos < e.splitPlane.pos) || (splitPlane.pos == e.splitPlane.pos && type < e.type));
    }

};

// best spliting plane using SAH heuristic
void Node::findPlane(const std::vector<std::shared_ptr<Object>> T, const BBOX& V, int depth,
               SplitPlane& p_est, float& C_est, PlaneSide& pside_est) const {
    // static int count = 0;
    C_est = INFINITY;
    for(int k=0; k<3; ++k) {
        std::vector<Event> events;
        events.reserve(T.size()*2);
        for(auto pit = T.begin(); pit != T.end(); pit++) {
            std::shared_ptr<Object> t = *pit;
            BBOX B = clipTriangleToBox(t, V);
            if(B.isPlanar()) {
                events.push_back(Event(t, k, B.min[k], Event::lyingOnPlane));
            } else {
                events.push_back(Event(t, k, B.min[k], Event::startingOnPlane));
                events.push_back(Event(t, k, B.max[k], Event::endingOnPlane));
            }
        }
        std::sort(events.begin(), events.end());
        int NL = 0, NP = 0, NR = T.size();
        for(std::vector<Event>::size_type Ei = 0; Ei < events.size(); ++Ei) {
            const SplitPlane& p = events[Ei].splitPlane;
            int pLyingOnPlane = 0, pStartingOnPlane = 0, pEndingOnPlane = 0;
            while(Ei < events.size() && events[Ei].splitPlane.pos == p.pos && events[Ei].type == Event::endingOnPlane) {
                ++pEndingOnPlane;
                Ei++;
            }
            while(Ei < events.size() && events[Ei].splitPlane.pos == p.pos && events[Ei].type == Event::lyingOnPlane) {
                ++pLyingOnPlane;
                Ei++;
            }
            while(Ei < events.size() && events[Ei].splitPlane.pos == p.pos && events[Ei].type == Event::startingOnPlane) {
                ++pStartingOnPlane;
                Ei++;
            }
            NP = pLyingOnPlane;
            NR -= pLyingOnPlane;
            NR -= pEndingOnPlane;
            float C;
            PlaneSide pside = UNKNOWN;
            SAH(p, V, NL, NR, NP, C, pside);
            if(C < C_est) {
                C_est = C;
                p_est = p;
                pside_est = pside;
            }
            NL += pStartingOnPlane;
            NL += pLyingOnPlane;
            NP = 0;
        }
    }
}


void Node::sortTriangles(std::vector<std::shared_ptr<Object>> T, const SplitPlane& p, const PlaneSide& pside, std::vector<std::shared_ptr<Object>> TL, std::vector<std::shared_ptr<Object>> TR) const {
    for(auto pit = T.begin(); pit != T.end(); pit++) {
        std::shared_ptr<Object> t = *pit;
        BBOX tbox = t->get_bbox();
        if(tbox.min[p.axis] == p.pos && tbox.max[p.axis] == p.pos) {
            if(pside == LEFT)
                TL.push_back(t);
            else if(pside == RIGHT)
                TR.push_back(t);
            else
                std::cout << "ERROR WHILE SORTING TRIANLGES" << std::endl;
        } else {
            if(tbox.min[p.axis] < p.pos)
                TL.push_back(t);
            if(tbox.max[p.axis] > p.pos)
                TR.push_back(t);
        }
    }
}

int maxdepth = 0;
int nnodes = 0;
Node* Node::RecBuild(std::vector<std::shared_ptr<Object>> triangles, const BBOX &V, int depth, const SplitPlane& prev_plane){
    ++nnodes; // DEBUG ONLY
    if(depth > maxdepth) maxdepth = depth; // DEBUG ONLY

    SplitPlane p;
    float Cp;
    PlaneSide pside;
    findPlane(triangles, V, depth, p, Cp, pside);
    if(isDone(triangles.size(), Cp) || p == prev_plane) // NOT IN PAPER
    {
        // Leaf node
        Node* leafnode = new Node();
        leafnode->objects = triangles;
        leafnode->bounds = V;
        leafnode->is_leaf = true;
        std::cout << "Leaf node. Depth: " << depth << ". Number of trianlges: "<< triangles.size() << std::endl;
        return leafnode;
    }
    BBOX VL, VR;
    splitBox(V, p, VL, VR); // TODO: avoid doing this step twice
    std::vector<std::shared_ptr<Object>> TL, TR;
    sortTriangles(triangles, p, pside, TL, TR);
    // Inner node
    Node * innerNode = new Node();
    innerNode->splitPlane = p;
    innerNode->bounds = V;
    innerNode->is_leaf = false;
    innerNode->left = RecBuild(TL, VL, depth+1, p);
    innerNode->right = RecBuild(TR, VR, depth+1, p);
    return innerNode;
}


bool Node::intersect_tree(std::shared_ptr<Ray> ray) const
{
    return false;
}