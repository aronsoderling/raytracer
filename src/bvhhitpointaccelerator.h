/*
*  BVHHitpointAccelerator.h
*  prTracer
*
*  Copyright 2011 Lund University. All rights reserved.
*
*/

#ifndef BVHHITPOINTACCELERATOR_H
#define BVHHITPOINTACCELERATOR_H

#include "rayaccelerator.h"
#include "bvhnode.h"
#include "photonmapper.h"

class BVHHitpointAccelerator
{
private:
	std::vector<Hitpoint*> objs;
	BVHNode* root;
	std::vector<BVHNode*> nodes;

public:
	virtual void build(const std::vector<Hitpoint*>& objects);
	void build_recursive(int left_index, int right_index, BVHNode* node, int depth);
	virtual bool intersect(const Point3D& point);
	void print_rec(BVHNode& node, int depth);
	void print();
};

#endif