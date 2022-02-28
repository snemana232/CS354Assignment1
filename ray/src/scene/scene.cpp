#include <cmath>

#include "scene.h"
#include "light.h"
#include "kdTree.h"
#include "../ui/TraceUI.h"
#include <glm/gtx/extended_min_max.hpp>
#include <iostream>
#include <glm/gtx/io.hpp>

using namespace std;

bool Geometry::intersect(ray& r, isect& i) const {
	double tmin, tmax;
	if (hasBoundingBoxCapability() && !(bounds.intersect(r, tmin, tmax))) return false;
	// Transform the ray into the object's local coordinate space
	glm::dvec3 pos = transform->globalToLocalCoords(r.getPosition());
	glm::dvec3 dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = glm::length(dir);
	dir = glm::normalize(dir);
	// Backup World pos/dir, and switch to local pos/dir
	glm::dvec3 Wpos = r.getPosition();
	glm::dvec3 Wdir = r.getDirection();
	r.setPosition(pos);
	r.setDirection(dir);
	bool rtrn = false;
	if (intersectLocal(r, i))
	{
		// Transform the intersection point & normal returned back into global space.
		i.setN(transform->localToGlobalCoordsNormal(i.getN()));
		i.setT(i.getT()/length);
		rtrn = true;
	}
	// Restore World pos/dir
	r.setPosition(Wpos);
	r.setDirection(Wdir);
	return rtrn;
}

bool Geometry::hasBoundingBoxCapability() const {
	// by default, primitives do not have to specify a bounding box.
	// If this method returns true for a primitive, then either the ComputeBoundingBox() or
    // the ComputeLocalBoundingBox() method must be implemented.

	// If no bounding box capability is supported for an object, that object will
	// be checked against every single ray drawn.  This should be avoided whenever possible,
	// but this possibility exists so that new primitives will not have to have bounding
	// boxes implemented for them.
	return false;
}

void Geometry::ComputeBoundingBox() {
    // take the object's local bounding box, transform all 8 points on it,
    // and use those to find a new bounding box.

    BoundingBox localBounds = ComputeLocalBoundingBox();
        
    glm::dvec3 min = localBounds.getMin();
    glm::dvec3 max = localBounds.getMax();

    glm::dvec4 v, newMax, newMin;

    v = transform->localToGlobalCoords( glm::dvec4(min[0], min[1], min[2], 1) );
    newMax = v;
    newMin = v;
    v = transform->localToGlobalCoords( glm::dvec4(max[0], min[1], min[2], 1) );
    newMax = glm::max(newMax, v);
    newMin = glm::min(newMin, v);
    v = transform->localToGlobalCoords( glm::dvec4(min[0], max[1], min[2], 1) );
    newMax = glm::max(newMax, v);
    newMin = glm::min(newMin, v);
    v = transform->localToGlobalCoords( glm::dvec4(max[0], max[1], min[2], 1) );
    newMax = glm::max(newMax, v);
    newMin = glm::min(newMin, v);
    v = transform->localToGlobalCoords( glm::dvec4(min[0], min[1], max[2], 1) );
    newMax = glm::max(newMax, v);
    newMin = glm::min(newMin, v);
    v = transform->localToGlobalCoords( glm::dvec4(max[0], min[1], max[2], 1) );
    newMax = glm::max(newMax, v);
    newMin = glm::min(newMin, v);
    v = transform->localToGlobalCoords( glm::dvec4(min[0], max[1], max[2], 1) );
    newMax = glm::max(newMax, v);
    newMin = glm::min(newMin, v);
    v = transform->localToGlobalCoords( glm::dvec4(max[0], max[1], max[2], 1) );
    newMax = glm::max(newMax, v);
    newMin = glm::min(newMin, v);
		
    bounds.setMax(glm::dvec3(newMax));
    bounds.setMin(glm::dvec3(newMin));
}

Scene::Scene()
{
	ambientIntensity = glm::dvec3(0, 0, 0);
}

Scene::~Scene()
{
}

void Scene::add(Geometry* obj) {
	obj->ComputeBoundingBox();
	sceneBounds.merge(obj->getBoundingBox());
	objects.emplace_back(obj);
}

void Scene::add(Light* light)
{
	lights.emplace_back(light);
}


// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect(ray& r, isect& i) const {
	double tmin = 0.0;
	double tmax = 0.0;
	bool have_one = false;
	for(const auto& obj : objects) {
		isect cur;
		if( obj->intersect(r, cur) ) {
			if(!have_one || (cur.getT() < i.getT())) {
				i = cur;
				have_one = true;
			}
		}
	}
	if(!have_one)
		i.setT(1000.0);
	// if debugging,
	if (TraceUI::m_debug)
	{
		addToIntersectCache(std::make_pair(new ray(r), new isect(i)));
	}
	return have_one;
}

TextureMap* Scene::getTexture(string name) {
	auto itr = textureCache.find(name);
	if (itr == textureCache.end()) {
		textureCache[name].reset(new TextureMap(name));
		return textureCache[name].get();
	}
	return itr->second.get();
}


//Build the tree
Node Node::buildTree(std::vector<Geometry *> objects, BoundingBox bb, int depth, int depthLimit, int leafSize) {
    if(objects.size() <= leafSize || ++depth == depthLimit) {
       return LeafNode(objects);
    }
   //get the Best Plane here 
   SplitPlane bestPlane = findBestSplitPlane(std::move(objects), bb);
   std::vector<Geometry*> left_list;
   std::vector<Geometry*> right_list;
   //add bounding box to this Node's member variables
   this->Boundary = bb;
   for ( std::vector<Geometry*>::const_iterator obj = objects.begin(); obj != objects.end(); obj++ ) {
    //add a left list
    if((  (*obj)-> getBoundingBox().getMin())[bestPlane.axis] < bestPlane.position) {
           left_list.push_back(*obj);
       }
    //add to right list
    if((  (*obj)-> getBoundingBox().getMax())[bestPlane.axis] > bestPlane.position) {
           right_list.push_back(*obj);
       }
       glm::dvec3 zero(0, 0, 0);
 
    if(( (*obj) -> getBoundingBox().getMax())[bestPlane.axis] == bestPlane.position && ((*obj)-> getBoundingBox().getMin())[bestPlane.axis] == bestPlane.position && glm::lessThan((*obj)->getNormal(), zero)[0]) {
           left_list.push_back(*obj);
       }
 
       else if(( (*obj) -> getBoundingBox().getMax())[bestPlane.axis] == bestPlane.position && ((*obj) -> getBoundingBox().getMin())[bestPlane.axis] == bestPlane.position && glm::greaterThanEqual((*obj)->getNormal(), zero)[0]) {
           right_list.push_back(*obj);
 
       }
   }
   if (right_list.empty() || left_list.empty()) {
       return LeafNode(objects);
   } else {
       //recursively set left and right nodes
       return SplitNode(bestPlane.position, bestPlane.axis, buildTree(left_list, bestPlane.left_bb, depth - 1, depthLimit, leafSize), buildTree(right_list, bestPlane.right_bb, depth - 1, depthLimit, leafSize));
   }
}
 
Node::SplitPlane Node::findBestSplitPlane(std::vector<Geometry *> objects, BoundingBox bb) {
   std::vector<SplitPlane> candidateList;
   for (int i = 0; i < 3; i++) {//iterate through x, y, z axis
       Node::SplitPlane p1(0, 0);
       Node::SplitPlane p2(0, 0);
       for ( std::vector<Geometry*>::const_iterator obj = objects.begin(); obj != objects.end(); obj++ ) {
           p1.axis = i;
           p1.position = ( (*obj)-> getBoundingBox().getMin())[i];
           p2.axis = i;
           p2.position = ( (*obj)-> getBoundingBox().getMax())[i];
           candidateList.push_back(p1);
           candidateList.push_back(p2);
       }
   }
 
   for (int i = 0; i < candidateList.size(); i++) {
       for ( std::vector<Geometry*>::const_iterator obj = objects.begin(); obj != objects.end(); obj++ ) {
           SplitPlane plane = candidateList[i];
           if(( (*obj)-> getBoundingBox().getMax())[plane.axis] <= plane.position) {
               plane.left_count++;
               BoundingBox temp = (*obj) -> getBoundingBox();
               plane.left_area += temp.area();
           }
           if(( (*obj)-> getBoundingBox().getMin())[plane.axis] > plane.position) {
               plane.right_count++;
               BoundingBox temp = (*obj)->getBoundingBox();
               plane.right_area += temp.area();
           }
           //Get left bounding box using our coordinate
           glm::dvec3 left_min = bb.getMin();
           glm::dvec3 left_max = bb.getMax();
           left_max[plane.axis] = plane.position;
           plane.left_bb = BoundingBox(left_min,left_max);
           //Get right bounding box using our coordinate
           glm::dvec3 right_min = bb.getMin();
           glm::dvec3 right_max = bb.getMax();
           right_min[plane.axis] = plane.position;
           plane.right_bb = BoundingBox(right_min, right_max);
       }
   }
 
   double minCost = INT_MAX;
   Node::SplitPlane bestPlane(0, 0);
    
   for (int i = 0; i < candidateList.size(); i++) {
       SplitPlane plane = candidateList[i];
        //SAM criteria: (chance of hitting left + channce of hitting right)/area
       double cost = (plane.left_count * plane.left_bb.area() + plane.right_count * plane.right_bb.area()) / bb.area();
       if (cost < minCost) {
           minCost = cost;
           bestPlane = plane;
       }
   }
   return bestPlane;
}


//This method is just here to compile C
bool Node::findIntersection(ray& r, isect& i, double t_min, double t_max) {
    return false;
}

//Split Node intersection
bool SplitNode::findIntersection(ray& r, isect& i, double t_min, double t_max) {
    //get left and right bounding boxes using position
    glm::dvec3 left_min = Boundary.getMin();
    glm::dvec3 left_max = Boundary.getMax();
    left_max[axis] = position;
    BoundingBox left_bb = BoundingBox(left_min, left_max);
    glm::dvec3 right_min = Boundary.getMin();
    glm::dvec3 right_max = Boundary.getMax();
    right_min[axis] = position;
    BoundingBox right_bb = BoundingBox(right_min, right_max);

    bool hits_left = left_bb.intersect(r, t_min, t_max);
    bool hits_right = right_bb.intersect(r, t_min, t_max);

       if (hits_left && !hits_right) {
           if (left->findIntersection(r, i, t_min, t_max)) { return true;}
       } else if (hits_right && !hits_left) {
           if (right->findIntersection(r, i, t_min, t_max)) {return true; }
       } else {
           if (left->findIntersection(r, i, t_min, t_max)) { return true; }
           if (right->findIntersection(r, i, t_min, t_max)) {return true; }
       }
   return false;
}
 
bool LeafNode::findIntersection(ray& r, isect& i, double t_min, double t_max) {
    for ( std::vector<Geometry*>::const_iterator obj = objList.begin(); obj != objList.end(); obj++ ) {
        isect c_i;
        if ((*obj)->intersect(r, c_i) && c_i.getT() >= t_min && c_i.getT() <= t_max) {
            i = c_i;
            return true;
        }
    }
   return false;
}


void Node::printTree(Node *thisTree) {
    if (thisTree == NULL) {
        return;
    }
    if (thisTree->left == NULL && thisTree->right == NULL) {
        std::cout << "Leaf Node" << endl;
    } else {
            printTree(thisTree->left);
            printTree(thisTree->right);
    }

}

