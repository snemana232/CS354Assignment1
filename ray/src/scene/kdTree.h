#pragma once
// Note: you can put kd-tree heree
#include "scene.h"
#include "bbox.h"
class Geometry;
class kdTree {
//Geometry* se;
private:
	kdTree* left;
	kdTree* right;
	std::vector<Geometry*> obj_list;
public:
	BoundingBox Boundary;
	Scene* scene;
	std::vector<glm::dvec3> coordinates;
	double dividing_plane;
	int dividing_dim;
	int dead;
	kdTree(){dead = -1;};
	kdTree(std::vector<Geometry*> objects, BoundingBox bb, int depth);
	bool intersect(const ray& r, isect& i, std::vector<Geometry*>& rlist);
	//kdTree() {scene = s;}
	//void setup();
	//void buildTree();

};

class Node { // virtual
public:
	Node();
};

class SplitNode : public Node {
public:
	double axis;
	double position;
	kdTree* left;
	kdTree* right;

	SplitNode(double axis, double position, kdTree* left, kdTree* right) {
		this->axis = axis;
		this->position = position;
		this->left = left;
		this->right = right;
	};
};

class LeafNode : public Node {
public:
	std::vector<Geometry*> objList;

	LeafNode(std::vector<Geometry*> objList) {
		this->objList = objList;
	};
};

class SplitPlane {
    public:
    int axis;
    double position;
    double left_count;
    double right_count;
    BoundingBox left_bb;
    BoundingBox right_bb;

    SplitPlane();

    SplitPlane(int axis, double position) {
        this->axis = axis;
        this->position = position;
    };

};

/*build tree

1. Check costs.
for(int i = 0; i < scene.objects, list of division planes)// the object list is sorted by x,y,z axis
{
	cost = Sa/S
	if (cost > maxcost)
	{
		maxcost = cost;
		maxcost_index = cost;
	}
}

That's the division
get objects on near and far side
set into the near and far node
pass through build tree with the new node and the list of objects
recurse
do the same with far side
recurse
done
*/
