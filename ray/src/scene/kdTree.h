#pragma once
// Note: you can put kd-tree heree
#include "scene.h"
#include "bbox.h"
#include <iostream>

class SplitNode;
class LeafNode;
class Geometry; 

//Parent class to SplitNodes and LeafNodes
class Node {
public:
	Node *left;
	Node *right;

	Node(){

	};

	//SplitPlane class
	class SplitPlane {
		public:
		int axis;
		double position;
		double left_count;
		double right_count;
		double left_area;
		double right_area;
		BoundingBox left_bb;
		BoundingBox right_bb;

		SplitPlane(int axis, double position) {
			this->axis = axis;
			this->position = position;
		};

	};
	//This node's boundary
	BoundingBox Boundary;
	bool findIntersection(ray& r, isect& i, double t_min, double t_max);
	Node buildTree(std::vector<Geometry*> objects, BoundingBox bb, int depth, int depthLimit, int leafSize);
	SplitPlane findBestSplitPlane(std::vector<Geometry*> objList, BoundingBox bb);
	void printTree(Node *thisTree);

};




class SplitNode: public Node {
public:
	double position;
	double axis;


	SplitNode(double position, double axis, Node left, Node right) {
		this->position = position;
		this->axis = axis;
		this->left = &left;
		this->right = &right;
	};

	bool findIntersection(ray& r, isect& i, double t_min, double t_max);
};

class LeafNode: public Node {
public:
	std::vector<Geometry*> objList;
	LeafNode(std::vector<Geometry*> objList) {
		this->objList = objList;
		this->left = NULL;
		this->right = NULL;
	};
	bool findIntersection(ray& r, isect& i, double t_min, double t_max);
};



