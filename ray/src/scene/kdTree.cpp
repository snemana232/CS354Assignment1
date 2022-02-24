// Note: you can put kd-tree heree
#include "kdTree.h"
#include <iostream>

// kdTree::kdTree(std::vector<Geometry*> objects, BoundingBox bb, int depth) {
// 	Boundary = bb;
// 	//scene = s;
// 	if(objects.size() == 0)
// 	{
// 		return;
// 	}
// 	if (depth == 0)
// 	{
// 		std::cout << "BoundingBox Shape " << bb.getMin()[0] << " " << bb.getMin()[1] << " " << bb.getMin()[2] << " x "  << bb.getMax()[0] << " " << bb.getMax()[1] << " " << bb.getMax()[2] << std::endl;
// 		std::cout << "Amount of objects in it. " << objects.size() << std::endl;
// 		obj_list = objects;
// 		return;
// 	}
// 	for(std::vector<Geometry*>::const_iterator begin = objects.begin(); begin !=  objects.end(); begin++)
// 	{
// 		coordinates.push_back( (*begin) -> getBoundingBox().getMin() );
// 		coordinates.push_back( (*begin) -> getBoundingBox().getMax() );
// 	}

// 	//x axis;
// 	int axis_of_choice = 0;
// 	double maxcost = 1000000000;
// 	double maxcost_index = 0;
// 	double maxplane = 0;



// 	std::vector<Geometry*> left_list;
// 	std::vector<Geometry*> right_list;
// 	for(int dimension = 0; dimension < 3; dimension++)
// 	{
// 		for(std::vector<glm::dvec3>::iterator begin = coordinates.begin(); begin != coordinates.end(); begin++)
// 		{
// 			double plane = (*begin)[dimension];
// 			int left_cost = 0;
// 			int right_cost = 0;
// 			int n_a = 0;
// 			double s_a = 0;
// 			int n_b = 0;
// 			double s_b = 0;
// 			double s_total = 0;


// 			for(std::vector<Geometry*>::const_iterator begin2 = objects.begin(); begin2 !=  objects.end(); begin2++)
// 			{
// 				//check if they are on the left or right side
// 				if(((*begin2) -> getBoundingBox().getMax())[dimension] <= plane) //check left
// 				{
// 					n_a++;
// 					BoundingBox temp_box = (*begin2) -> getBoundingBox();
// 					s_a += temp_box.area();
// 					//near.push_back(*begin2);

// 				}
// 				if(((*begin2) -> getBoundingBox().getMin())[dimension] >  plane) //check right
// 				{
// 					n_b++;
// 					BoundingBox temp_box = (*begin2) -> getBoundingBox();
// 					s_b += temp_box.area();
// 										//far.push_back(*begin2);
// 				}
// 				BoundingBox temp_box = (*begin2) -> getBoundingBox();
// 				s_total += temp_box.area();
// 			}

// 			double cost = n_a*s_a + n_b*s_b;
// 			//std::cout << "Cost calculated " << cost << " at " << plane << " " << dimension << std::endl;    
// 			if(cost < maxcost)
// 			{
// 				maxcost = cost;
// 				maxcost_index = dimension;
// 				maxplane = plane;
// 			}
// 		}
// 		//std::cout << dimension << std::endl;
// 	}
// 	//std::cout << "*** DECISION *** >> " << maxcost << " at " << maxplane << " " << maxcost_index <<  std::endl;
// 	for(std::vector<Geometry*>::const_iterator begin2 = objects.begin(); begin2 !=  objects.end(); begin2++)
// 	{
// 		//check if they are on the left or right side
// 		if(((*begin2) -> getBoundingBox().getMin())[maxcost_index] <= maxplane) //check left
// 		{
// 			left_list.push_back(*begin2);
// 		}
// 		if(((*begin2) -> getBoundingBox().getMax())[maxcost_index] >  maxplane) //check right
// 		{
// 			right_list.push_back(*begin2);
// 		}
// 	}
// 	dividing_plane = maxplane;
// 	dividing_dim   = maxcost_index;
// 	glm::dvec3 leftbound_min = bb.getMin();
// 	glm::dvec3 leftbound_max = bb.getMax();
// 	leftbound_max[maxcost_index] = maxplane;

// 	glm::dvec3 rightbound_min = bb.getMin();
// 	glm::dvec3 rightbound_max = bb.getMax();
// 	rightbound_min[maxcost_index] = maxplane;

// 	BoundingBox leftBound = BoundingBox(leftbound_min,leftbound_max);
// 	BoundingBox rightBound = BoundingBox(rightbound_min,rightbound_max);

// 	left = new kdTree{left_list, leftBound, depth - 1};
// 	std::cout << " Left completed " << depth << std::endl;
// 	right = new kdTree{right_list, rightBound, depth - 1};
// 	std::cout << " Right completed " << depth << std::endl;
// }

// bool kdTree::intersect(const ray& r, isect& i, std::vector<Geometry*>& rlist)
// 	{
// 		double t_min;
// 		double t_max;
// 		bool intersected = Boundary.intersect(r,t_min,t_max);
		
// 		glm::dvec3 p = r.getPosition();
// 		glm::dvec3 d = r.getDirection();
// 		double t_star = -p[dividing_dim]/d[dividing_dim];



// 		if(dead == -1)
// 		{
// 			bool intersection;
// 			std::cout << "IN THE ZONE obj_list size " << obj_list.size() << "\n";
// 			for(std::vector<Geometry*>::const_iterator begin = obj_list.begin(); begin != obj_list.end(); begin++)
// 			{
// 				rlist.push_back((*begin));
// 			}
// 			return true;

// 		}
// 		if( t_star <= RAY_EPSILON ) {
// 			return false;
// 		}

// 		if(t_max < t_star)
// 		{
// 			std::cout << "HIDARI \n";

// 			left -> intersect(ray(r.at(t_min),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i, rlist);
// 			return true;
// 		}

// 		if(t_min < t_star < t_max)
// 		{
// 			std::cout << "L & R \n";

// 			left -> intersect(ray(r.at(t_min),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i, rlist);
// 			right -> intersect(ray(r.at(t_star),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i, rlist);
// 			return true;
// 		}
// 		if(t_star < t_min)
// 		{
// 			std::cout << "MIGI \n";


// 			right -> intersect(ray(r.at(t_min),r.getDirection(),0,0,glm::dvec3{0,0,0},ray::VISIBILITY),i, rlist);
// 			return true;
// 		}

// 		std::cout << "NOT EVEN. tmin = " << t_min <<  "  tmax = " << t_max << " t_star " << t_star << std::endl;

// 		return false;
// 	}


Node buildTree(std::vector<Geometry*> objects, BoundingBox bb, int depth, int depthLimit, int leafSize) {
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
        return SplitNode(bestPlane.position, bestPlane.axis, BoundingBox(bestPlane));
    }
}

SplitPlane findBestSplitPlane(std::vector<Geometry*> objList, BoundingBox bb) {
    std::vector<SplitPlane> candidateList = {};
    for (int i = 0; i < 3; i++) {//iterate through x, y, z axis
        SplitPlane p1;
        SplitPlane p2;
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
            BoundingBox leftBound = BoundingBox(left_min,left_max);
            plane.left_bb = leftBound;
            glm::dvec3 right_min = bb.getMin();
            glm::dvec3 right_max = bb.getMax();
            right_min[plane.axis] = plane.position;
            BoundingBox rightBound = BoundingBox(right_min, right_max);
            plane.right_bb = rightBound;
        }
    }

    double minCost = INT_MAX;
    SplitPlane bestPlane;

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
