#include "material.h"
#include "../ui/TraceUI.h"
#include "light.h"
#include "ray.h"
extern TraceUI* traceUI;

#include <glm/gtx/io.hpp>
#include <iostream>
#include "../fileio/images.h"

using namespace std;
extern bool debugMode;

Material::~Material()
{
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene* scene, const ray& r, const isect& i) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and
	// shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.
	//	if( debugMode )
	//		std::cout << "Debugging Phong code..." << std::endl;

	// When you're iterating through the lights,
	// you'll want to use code that looks something
	// like this:
	//
	// for ( const auto& pLight : scene->getAllLights() )
	// {
	//              // pLight has type unique_ptr<Light>
	// 		.
	// 		.
	// 		.
	// }
	//Phong shading - ColorC is intensity
	//Ambient term
	glm::dvec3 colorC = ke(i) + ka(i)*scene->ambient();
		//Contributions of lights
		for ( const auto& pLight : scene->getAllLights())
		{
		//I_i value
		glm::dvec3 attenuation = pLight->distanceAttenuation(r.at(i.getT())) * pLight->shadowAttenuation(r, r.at(i.getT()));
		//Diffuse Term
		glm::dvec3 L = pLight->getDirection(r.at(i.getT())); 
		double L_N = glm::dot(L, i.getN());
		if (0.0 >= L_N) {
			L_N = 0.0;
		}
		glm::dvec3 I_diffuse = kd(i)*L_N;
		//Specular Term
		glm::dvec3 R = glm::normalize(glm::reflect(r.getDirection(), i.getN()));
		glm::dvec3 V =  glm::normalize(scene->getCamera().getEye() - r.at(i.getT()));
		double R_V = glm::dot(R, V);
		if (R_V <= 0) {
			R_V = 0.0;
		}
		glm::dvec3 I_specular = ks(i) * pow(R_V, shininess(i));
		//Add in contributions
		colorC += attenuation * (I_specular + I_diffuse);
		}	
	return colorC;
}

TextureMap::TextureMap(string filename)
{
	data = readImage(filename.c_str(), width, height);
	if (data.empty()) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("'.");
		throw TextureMapException(error);
	}
}

glm::dvec3 TextureMap::getMappedValue(const glm::dvec2& coord) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.
	double x = coord[0] * width;
	double y = coord[1] * height;
	//error checking to make sure out of bounds
	//coordinates are not given
	if (x >= width) x = width - 1;
	if (y >= height) y = height - 1;
	return getPixelAt(x, y);
}

glm::dvec3 TextureMap::getPixelAt(int x, int y) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	int pos = ((y * width) + x) * 3;
	return glm::dvec3(data[pos]/255.0, data[pos + 1]/255.0, data[pos + 2]/255.0);
}

glm::dvec3 MaterialParameter::value(const isect& is) const
{
	if (0 != _textureMap)
		return _textureMap->getMappedValue(is.getUVCoordinates());
	else
		return _value;
}

double MaterialParameter::intensityValue(const isect& is) const
{
	if (0 != _textureMap) {
		glm::dvec3 value(
		        _textureMap->getMappedValue(is.getUVCoordinates()));
		return (0.299 * value[0]) + (0.587 * value[1]) +
		       (0.114 * value[2]);
	} else
		return (0.299 * _value[0]) + (0.587 * _value[1]) +
		       (0.114 * _value[2]);
}
