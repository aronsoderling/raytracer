/*
*  BVHHitpointAccelerator.cpp
*  prTracer
*
*/

#include "bvhhitpointaccelerator.h"
#include <algorithm> 
#include <iomanip>
#include <stack>

using namespace std;

void getAABB(Hitpoint* hp, AABB& bb){
	bb = AABB();

	for (int i = 0; i < 8; i++) {
		Vector3D p2((float)(2 * (i & 1) - 1) * hp->radius,
			(float)(2 * ((i & 2) >> 1) - 1) * hp->radius,
			(float)(2 * ((i & 4) >> 2) - 1) * hp->radius);
		Point3D p = hp->is.mPosition + p2;
		bb.include(p);
	}
}

class ComparePrimitives {
public:
	bool operator() (Hitpoint *a, Hitpoint *b) {
		AABB box;
		getAABB(a, box);
		float ca = (box.mMax(sort_dim) + box.mMin(sort_dim)) * 0.5f;
		getAABB(b, box);
		float cb = (box.mMax(sort_dim) + box.mMin(sort_dim)) * 0.5f;
		return ca < cb;
	}

	int sort_dim;
};

bool intersectAABB(const Point3D& point, const AABB& bb){
	return point.x >= bb.mMin.x && point.x <= bb.mMax.x &&
		point.y >= bb.mMin.y && point.y <= bb.mMax.y &&
		point.z >= bb.mMin.z && point.z <= bb.mMax.z;
}

void BVHHitpointAccelerator::build(const vector<Hitpoint*>& objects)
{
	AABB worldBox;
	for (Hitpoint* o : objects){
		AABB temp;
		getAABB(o, temp);
		worldBox.include(temp);
		objs.push_back(o);
	}
	root = new BVHNode();
	root->setAABB(worldBox);
	nodes.push_back(root);
	build_recursive(0, objs.size(), root, 0);
	//print();
}
void BVHHitpointAccelerator::build_recursive(int left_index, int right_index, BVHNode* node, int depth){
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
		getAABB(objs[split_index], aabb);
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
		getAABB(objs[i], aabb);
		left.include(aabb);
	}

	AABB right;
	for (int i = split_index; i < right_index; ++i){
		getAABB(objs[i], aabb);
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

bool BVHHitpointAccelerator::intersect(const Point3D& point, Hitpoint& hp)
{
	float tmin, tmax;
	stack<BVHNode*> nodeStack;
	nodeStack.push(root);

	while (!nodeStack.empty()){
		BVHNode* node = nodeStack.top();
		nodeStack.pop();

		AABB bb = node->getAABB();

		if (intersectAABB(point, bb)){

			if (node->isLeaf()){
				for (int i = node->getIndex(); i < node->getIndex() + node->getNObjs(); ++i){
					Hitpoint* obj = objs[i];
					Vector3D dist(point - obj->is.mPosition);
					if ( dist.length2() <= obj->radius * obj->radius ){
						hp = *obj;
						return true;
					}
				}
			}
			else{
				BVHNode* left = nodes[node->getIndex()];
				BVHNode* right = nodes[node->getIndex() + 1];

				if (intersectAABB(point, right->getAABB())){
					nodeStack.push(right);
				}
				if (intersectAABB(point, left->getAABB())){
					nodeStack.push(left);
				}
			}
		}
	}
	return false;
}


void BVHHitpointAccelerator::print_rec(BVHNode& node, int depth)
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
void BVHHitpointAccelerator::print()
{
	cout << "Printing nodes..." << endl;
	cout << "World Bounds: " << endl;
	cout << "Min: " << root->getAABB().mMin;
	cout << "Max: " << root->getAABB().mMax << endl;
	print_rec(*root, 1);
}
