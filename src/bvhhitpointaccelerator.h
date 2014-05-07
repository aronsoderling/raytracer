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

struct Hitpoint {
	Intersection is;
	int pixelX, pixelY;
	float pixelWeight;
	float radius;
	Color directIllumination;
	float photonCount = 0;
	int newPhotonCount = 0;
	Color totalFlux = Color(0.0f, 0.0f, 0.0f);
};

class BVHHitpointAccelerator
{
private:
	BVHNode* root;
	std::vector<BVHNode*> nodes;

public:
	std::vector<Hitpoint*> objs;

	virtual void build(const std::vector<Hitpoint*>& objects);
	void build_recursive(int left_index, int right_index, BVHNode* node, int depth);
	virtual bool intersect(const Point3D& point, Hitpoint& hp);
	void print_rec(BVHNode& node, int depth);
	void print();
};

#endif