/*
*  bvhaccelerator.h
*  prTracer
*
*
*/

#ifndef BVHACCELERATOR_H
#define BVHACCELERATOR_H

#include "rayaccelerator.h"

class BVHAccelerator : public RayAccelerator
{
private:
	std::vector<Intersectable*> objects;

public:
	virtual void build(const std::vector<Intersectable*>& objects);
	virtual bool intersect(const Ray& ray);
	virtual bool intersect(const Ray& ray, Intersection& is);
};

#endif
