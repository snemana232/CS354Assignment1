#pragma once
// Note: you can put kd-tree heree
#include "scene.h"
#include "bbox.h"


class SplitNode;
class LeafNode;

class Node {


	private:
	Node* left;
	Node* right;
public:
	class SplitPlane {
		public:
		int axis;
		double position;
		double left_count;
		double right_count;
		BoundingBox left_bb;
		BoundingBox right_bb;

		SplitPlane(int axis, double position) {
			this->axis = axis;
			this->position = position;
		};

	};
	BoundingBox Boundary;
	Scene* scene;
	Node buildTree(std::vector<Geometry*> objects, BoundingBox bb, int depth, int depthLimit, int leafSize);
	SplitPlane findBestSplitPlane(std::vector<Geometry*> objList, BoundingBox bb);
	bool intersect(const ray& r, isect& i, std::vector<Geometry*>& rlist);

};




class SplitNode: public Node {
public:
	double position;
	double axis;
	Node left;
	Node right;

	SplitNode(double position, double axis, Node left, Node right) {
		this->position = position;
		this->axis = axis;
		this->left = left;
		this->right = right;
	};
};

class LeafNode: public Node {
public:
	std::vector<Geometry*> objList;

	LeafNode(std::vector<Geometry*> objList) {
		this->objList = objList;
	};
};



