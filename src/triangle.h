/*
 *  triangle.h
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-02-22.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "intersectable.h"

class Mesh;

/**
 * Class representing a single triangle. 
 * This class is used by Mesh to represent the triangles in the mesh.
 * It has functions for computing the triangle's face normal, area,
 * and for intersection testing with a ray.
 */
class Triangle : public Intersectable
{	
public:
	/// \cond INTERNAL_CLASS
	
	/// Internal class representing the vertex indices.
	struct vertex
	{
		int p;		///< Position index.
		int n;		///< Normal index.
		int t;		///< Auxiliary index (texture coordinate).
	};
	
	/// \endcond // INTERNAL_CLASS

	Triangle();
	Triangle(Mesh* owner, const vertex& vtx1, const vertex& vtx2, const vertex& vtx3, Material *mtl);
	
	Vector3D getFaceNormal() const;
	float getArea() const;
	void prepare();

	// Implementation of the Intersectable interface:
	bool intersect(const Ray& ray) const;
	bool intersect(const Ray& ray, Intersection& isect) const;
	void getAABB(AABB& bb) const;
	UV calculateTextureDifferential(const Point3D& p, const Vector3D& dp) const;
	Vector3D calculateNormalDifferential(const Point3D& p, const Vector3D& dp, bool isFrontFacing) const;

	const Point3D& getVtxPosition(int i) const;
	const Vector3D& getVtxNormal(int i) const;
	const UV& getVtxTexture(int i) const;
	Material *getMaterial() const {return mMaterial;}

protected:
	Vector3D mPlanes[3];
	Vector3D mPlaneOffsets;
	Mesh* mMesh;				///< Ptr to the mesh this triangle belongs to.
	vertex mVtx[3];				///< The three vertices of the triangle.	
	Material *mMaterial;		///< Material of the triangle.

	friend class Mesh;
};

#endif
