/*
*  bvhaccelerator.h
*  prTracer
*
*  Copyright 2011 Lund University. All rights reserved.
*
*/

#ifndef BVHNODE_H
#define BVHNODE_H

#include "aabb.h"

class BVHNode {
private:
	AABB bbox;
	bool leaf = false;
	unsigned int n_objs;
	unsigned int index; // if leaf == false: index to left child node, 
	// else if leaf == true: index to first Intersectable in Objs vector 
public:
	void setAABB(AABB& bbox_) { bbox = bbox_; }
	void makeLeaf(unsigned int index_, unsigned int n_objs_) { leaf = true;  index = index_; n_objs = n_objs_; }
	void makeNode(unsigned int left_index_, unsigned int n_objs_) { index = left_index_; n_objs = n_objs_; }
	// n_objs in makeNode is for debug purposes only, and may be omitted later on 

	bool isLeaf() { return leaf; }
	unsigned int getIndex() { return index; }
	unsigned int getNObjs() { return n_objs; }
	AABB &getAABB() { return bbox; };
};
#endif