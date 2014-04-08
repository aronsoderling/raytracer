/*
*  bvhaccelerator.h
*  prTracer
*
*  Copyright 2011 Lund University. All rights reserved.
*
*/

#ifndef BVHACCELERATOR_H
#define BVHACCELERATOR_H

#include "rayaccelerator.h"
#include "bvhnode.h"

class BVHAccelerator : public RayAccelerator
{
private:
	std::vector<Intersectable*> objs;
	BVHNode* root;
	std::vector<BVHNode*> nodes;

public:
	virtual void build(const std::vector<Intersectable*>& objects);
	void build_recursive(int left_index, int right_index, BVHNode* node, int depth);
	virtual bool intersect(const Ray& ray);
	virtual bool intersect(const Ray& ray, Intersection& is);
	void print_rec(BVHNode& node, int depth);
	void print();
};

#endif