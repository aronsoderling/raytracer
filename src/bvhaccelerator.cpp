/*
*  bvhaccelerator.cpp
*  prTracer
*
*/

#include "bvhaccelerator.h"
#include <algorithm> 
#include <iomanip>
#include <stack>

using namespace std;

class ComparePrimitives {
public:
	bool operator() (Intersectable *a, Intersectable *b) {
		AABB box;
		a->getAABB(box);
		float ca = (box.mMax(sort_dim) + box.mMin(sort_dim)) * 0.5f;
		b->getAABB(box);
		float cb = (box.mMax(sort_dim) + box.mMin(sort_dim)) * 0.5f;
		return ca < cb;
	}

	int sort_dim;
};


void BVHAccelerator::build(const vector<Intersectable*>& objects)
{
	AABB worldBox;
	for (Intersectable* o : objects){
		AABB temp;
		o->getAABB(temp);
		worldBox.include(temp);
		objs.push_back(o);
	}
	root = new BVHNode();
	root->setAABB(worldBox);
	nodes.push_back(root);
	build_recursive(0, objs.size(), root, 0);
	//print();
}
void BVHAccelerator::build_recursive(int left_index, int right_index, BVHNode* node, int depth){
	if ((right_index - left_index) <= 3 || depth == 20){// || (other termination criteria)){
		node->makeLeaf(left_index, right_index - left_index );
		return;
	}
	ComparePrimitives cmp;
	int largestAxis = node->getAABB().getLargestAxis();
	cmp.sort_dim = largestAxis;
	sort(objs.begin() + left_index, objs.begin() + right_index, cmp);
	float split;
	float currentMid;
	
	//node->getAABB().mMax(largestAxis);

	if (largestAxis == 0){
		split = (node->getAABB().mMax.x + node->getAABB().mMin.x) / 2.0f;
		currentMid = node->getAABB().mMin.x;
	}
	else if (largestAxis == 1){
		split = (node->getAABB().mMax.y + node->getAABB().mMin.y) / 2.0f;
		currentMid = node->getAABB().mMin.y;
	}else{
		split = (node->getAABB().mMax.z + node->getAABB().mMin.z) / 2.0f;
		currentMid = node->getAABB().mMin.z;
	}

	int split_index = left_index;
	AABB aabb;
	while (currentMid < split && split_index < right_index){
		objs[split_index]->getAABB(aabb);
		if (largestAxis == 0){
			currentMid = (aabb.mMax.x + aabb.mMin.x) / 2.0f;
		}
		else if (largestAxis == 1){
			currentMid = (aabb.mMax.y + aabb.mMin.y) / 2.0f;
		}
		else{
			currentMid = (aabb.mMax.z + aabb.mMin.z) / 2.0f;
		}
		++split_index;
	}
	//--split_index;

	if (split_index == left_index)
		++split_index;
	if (split_index == right_index)
		--split_index;

	//find split_index

	//calculate bounding boxes for left and right sides
	AABB left;
	for (int i = left_index; i < split_index; ++i){
		objs[i]->getAABB(aabb);
		left.include(aabb);
	}

	AABB right;
	for (int i = split_index; i < right_index; ++i){
		objs[i]->getAABB(aabb);
		right.include(aabb);
	}

	//Create two new nodes, leftNode and rightNode and assign bounding boxes 
	BVHNode* leftNode = new BVHNode();
	BVHNode* rightNode = new BVHNode();

	leftNode->setAABB(left);
	rightNode->setAABB(right);
	nodes.push_back(leftNode);
	nodes.push_back(rightNode);
	//Initiate current node as an interior node with leftNode and rightNode as children 
	node->makeNode(nodes.size() - 2, right_index - left_index);
	build_recursive(left_index, split_index, leftNode, depth + 1);
	build_recursive(split_index, right_index, rightNode, depth + 1);

}

bool BVHAccelerator::intersect(const Ray& ray)
{
	float tmin, tmax;
	Ray rayCopy(ray);
	stack<BVHNode*> nodeStack;
	nodeStack.push(root);

	while (!nodeStack.empty()){
		BVHNode* node = nodeStack.top();
		nodeStack.pop();

		if (node->getAABB().intersect(rayCopy, tmin, tmax)){
			if (node->isLeaf()){
				for (int i = node->getIndex(); i < node->getIndex() + node->getNObjs(); ++i){
					Intersectable* obj = objs[i];
					if (obj->intersect(rayCopy)){
						return true;
					}
				}
			}
			else{
				BVHNode* left = nodes[node->getIndex()];
				BVHNode* right = nodes[node->getIndex() + 1];

				if (right->getAABB().intersect(rayCopy, tmin, tmax)){
					nodeStack.push(right);
				}
				if (left->getAABB().intersect(rayCopy, tmin, tmax)){
					nodeStack.push(left);
				}
			}
		}
	}
	return false;
}

bool BVHAccelerator::intersect(const Ray& ray, Intersection& is)
{
	struct StackItem{
		BVHNode* node;
		float t;
	};
	float tmin, tmax;
	Ray rayCopy(ray);
	bool hit = false;
	stack<BVHNode*> nodeStack;
	nodeStack.push(root);

	while (!nodeStack.empty()){
		BVHNode* node = nodeStack.top();
		nodeStack.pop();

		if (node->getAABB().intersect(rayCopy, tmin, tmax)){
			if (node->isLeaf()){
				for (int i = node->getIndex(); i < node->getIndex() + node->getNObjs(); ++i){
					Intersectable* obj = objs[i];
					if (obj->intersect(rayCopy, is)){
						rayCopy.maxT = is.mHitTime;
						hit = true;
					}
				}
			}
			else{
				BVHNode* left = nodes[node->getIndex()];
				BVHNode* right = nodes[node->getIndex() + 1];

				if (right->getAABB().intersect(rayCopy, tmin, tmax)){
					nodeStack.push(right);
				}
				if (left->getAABB().intersect(rayCopy, tmin, tmax)){
					nodeStack.push(left);
				}
			}
		}
	}
	return hit;
}

void BVHAccelerator::print_rec(BVHNode& node, int depth)
{
	cout << setw(depth * 2) << ' ';
	if (node.isLeaf())
		cout << "Leaf<Primitives: " << node.getNObjs() << ", First primitive: " << node.getIndex() << ">" << endl;
	else {
		std::cout << "Node<Primitives: " << node.getNObjs() << ">" << endl;
		BVHNode* left = nodes[node.getIndex()];
		BVHNode* right = nodes[node.getIndex() + 1];
		print_rec(*left, depth + 1);
		print_rec(*right, depth + 1);
	}
}
void BVHAccelerator::print()
{
	cout << "Printing nodes..." << endl;
	cout << "World Bounds: " << endl;
	cout << "Min: " << root->getAABB().mMin;
	cout << "Max: " << root->getAABB().mMax << endl;
	print_rec(*root, 1);
}
