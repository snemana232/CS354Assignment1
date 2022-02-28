#include "cubeMap.h"
#include "ray.h"
#include "../ui/TraceUI.h"
#include "../scene/material.h"
extern TraceUI* traceUI;

glm::dvec3 CubeMap::getColor(ray r) const
{
	// YOUR CODE HERE
	// FIXME: Implement Cube Map here
	glm::dvec2 uv;
	int index;
	double n;

	if(abs(r.getDirection().x) > abs(r.getDirection().y) && abs(r.getDirection().x) > abs(r.getDirection().z)) {
		n = 0.5 / abs(r.getDirection().x);
		//+X
		if (r.getDirection().x > 0) {
			index = 0;
			uv = {-1 * r.getDirection().z, -1 * r.getDirection().y};
		//-X
		} else {
			index = 1;
			uv = {r.getDirection().z, -1 * r.getDirection().y};
		}
	} else if(abs(r.getDirection().y) > abs(r.getDirection().x) && abs(r.getDirection().y) > abs(r.getDirection().z)) {
		n = 0.5 / abs(r.getDirection().y);
		//+Y
		if (r.getDirection().y > 0) {
			index = 2;
			uv = {r.getDirection().x, r.getDirection().z};
		//-Y
		} else {
			index = 3;
			uv = {r.getDirection().x, -1 * r.getDirection().z};
		}
	} else {
		n = 0.5 / abs(r.getDirection().z);
		//+Z
		if (r.getDirection().z > 0) {
			index = 4;
			uv = {r.getDirection().x, -1 * r.getDirection().y};
		//-Z
		} else {
			index = 5;
			uv = {-1 * r.getDirection().x, -1 * r.getDirection().y};
		}
	}
	return tMap[index]->getMappedValue(glm::clamp((uv * n + 0.5), 0.0, 1.0));

}

CubeMap::CubeMap()
{
		
}

CubeMap::~CubeMap()
{
}

void CubeMap::setNthMap(int n, TextureMap* m)
{
	if (m != tMap[n].get())
		tMap[n].reset(m);
}
