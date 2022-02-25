// Note: you can put kd-tree here
#include "kdTree.h"
#include <iostream>


Node Node::buildTree(std::vector<Geometry*> objects, BoundingBox bb, int depth, int depthLimit, int leafSize) {
	if(objects.size() <= leafSize || ++depth == depthLimit) {
		return LeafNode(objects);
	}
	SplitPlane bestPlane = findBestSplitPlane(objects, bb);
    std::vector<Geometry*> left_list;
	std::vector<Geometry*> right_list;

	for(std::vector<Geometry*>::const_iterator begin = objects.begin(); begin !=  objects.end(); begin++) {

		if(((*begin) -> getBoundingBox().getMin())[bestPlane.axis] < bestPlane.position) {
            left_list.push_back(*begin);
        }

		if(((*begin) -> getBoundingBox().getMax())[bestPlane.axis] > bestPlane.position) {
            right_list.push_back(*begin);
        }

        glm::dvec3 zero(0, 0, 0);

		if(((*begin) -> getBoundingBox().getMax())[bestPlane.axis] == bestPlane.position && ((*begin) -> getBoundingBox().getMin())[bestPlane.axis] == bestPlane.position && glm::lessThan((*begin)->getNormal(), zero)[0]) {
            left_list.push_back(*begin);
        }

        else if(((*begin) -> getBoundingBox().getMax())[bestPlane.axis] == bestPlane.position && ((*begin) -> getBoundingBox().getMin())[bestPlane.axis] == bestPlane.position && glm::greaterThanEqual((*begin)->getNormal(), zero)[0]) {
            right_list.push_back(*begin);
        }
	}

    if (right_list.empty() || left_list.empty()) {
        return LeafNode(objects);
    } else {

        return SplitNode(bestPlane.position, bestPlane.axis, buildTree(left_list, bestPlane.left_bb, depth - 1, depthLimit, leafSize), buildTree(right_list, bestPlane.left_bb, depth - 1, depthLimit, leafSize));
    }
}

Node::SplitPlane Node::findBestSplitPlane(std::vector<Geometry*> objList, BoundingBox bb) {
    std::vector<SplitPlane> candidateList;
    for (int i = 0; i < 3; i++) {//iterate through x, y, z axis
        Node::SplitPlane p1(0, 0);
        Node::SplitPlane p2(0, 0);
        for(std::vector<Geometry*>::const_iterator begin = objList.begin(); begin !=  objList.end(); begin++) {
            p1.axis = i;
            p1.position = ((*begin) -> getBoundingBox().getMin())[i];
            p2.axis = i;
            p2.position = ((*begin) -> getBoundingBox().getMax())[i];
            candidateList.push_back(p1);
            candidateList.push_back(p2);
        }
    }

    for (int i = 0; i < candidateList.size(); i++) {
        for (std::vector<Geometry*>::const_iterator begin = objList.begin(); begin !=  objList.end(); begin++) {
            SplitPlane plane = candidateList[i];
            if(((*begin) -> getBoundingBox().getMax())[plane.axis] <= plane.position) {
                plane.left_count++;
                // BoundingBox temp = (*begin) -> getBoundingBox();
                // plane.left_area += temp.area();
            }
            if(((*begin) -> getBoundingBox().getMin())[plane.axis] <= plane.position) {
                plane.right_count++;

                // BoundingBox temp = (*begin) -> getBoundingBox();
                // plane.right_area += temp.area();
            }
            glm::dvec3 left_min = bb.getMin();
            glm::dvec3 left_max = bb.getMax();
            left_max[plane.axis] = plane.position;
            plane.left_bb = BoundingBox(left_min,left_max);
            glm::dvec3 right_min = bb.getMin();
            glm::dvec3 right_max = bb.getMax();
            right_min[plane.axis] = plane.position;
            BoundingBox rightBound = BoundingBox(right_min, right_max);
            plane.right_bb = rightBound;
        }
    }

    double minCost = INT_MAX;
    Node::SplitPlane bestPlane(0, 0);

    for (int i = 0; i < candidateList.size(); i++) {
        SplitPlane plane = candidateList[i];

        double cost = (plane.left_count * (plane.left_bb.area()/bb.area()) + plane.right_count * (plane.right_bb.area()/bb.area()));

        if (cost < minCost) {
            minCost = cost;
            bestPlane = plane;
        }
    }
    return bestPlane;
}

bool Node::intersect(const ray& r, isect& i, std::vector<Geometry*>& rlist) {
    return false;
}
