#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	glm::dvec3 d = getDirection(p);
	isect i;
	//send out a ray from p towards the light source
	ray TO_LIGHT(p, d, glm::dvec3(1,1,1), ray::SHADOW);
	//some intersection in between, decrease intensity returned
	if (scene->intersect(TO_LIGHT, i))	{
		return i.getMaterial().kt(i);
	}
	return glm::dvec3(1, 1, 1);

}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3& P) const
{

	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0
	double result = (1.0)/(constantTerm + linearTerm * glm::length(position - P) + quadraticTerm * pow(glm::length(position - P), 2));
	if (result >= 1) { //clamp this value
		result = 1.0; 
	}
	return result;
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	glm::dvec3 d = getDirection(p);
	isect i;
	//shoot out a ray from p towards the light source
	ray TO_LIGHT(p, d, glm::dvec3(1,1,1), ray::SHADOW);
	if (scene->intersect(TO_LIGHT, i) )	{
		//look for an intersection t that is not i.getT()
		if (glm::lessThan((position - TO_LIGHT.at(i.getT())) , (position - p))[0]) {
			//implementing shadows for transparent objects
			return i.getMaterial().kt(i);		
		}
	}
	return glm::dvec3(1, 1, 1);
}

#define VERBOSE 0

