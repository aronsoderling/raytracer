/*
 *  triangle.cpp
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-02-22.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "defines.h"
#include "ray.h"
#include "triangle.h"
#include "mesh.h"

#define EPSILON 0.000001

/// Triangle overlap distance to avoid problems at edges.
static const float overlap = 1e-3f;

/**
 * Constructor.
 */
Triangle::Triangle() : mMesh(0)
{
}

/**
 * Constructor initializing the triangle with the given vertices.
 */
Triangle::Triangle(Mesh* owner, const vertex& vtx1, const vertex& vtx2, const vertex& vtx3, Material *mtl) : mMesh(owner)
{
	mVtx[0] = vtx1;
	mVtx[1] = vtx2;
	mVtx[2] = vtx3;
	mMaterial = mtl;
}

/**
 * Compute the triangle's face normal from the position of its vertices.
 * The vertices are in counterclockwise order, which mean the normal can
 * be computed as: n = (v1-v0) x (v2-v0).
 */
Vector3D Triangle::getFaceNormal() const
{
	Vector3D e1 = getVtxPosition(1) - getVtxPosition(0);	// find edge v0-v1
	Vector3D e2 = getVtxPosition(2) - getVtxPosition(0);	// find edge v0-v2
	Vector3D n = e1 % e2;									// n = e1 x e2
	n.normalize();
	return n;
}

/**
 * Return the area of the triangle.
 */
float Triangle::getArea() const
{
	Vector3D e1 = getVtxPosition(1) - getVtxPosition(0);	// find edge v0-v1
	Vector3D e2 = getVtxPosition(2) - getVtxPosition(0);	// find edge v0-v2
	Vector3D n = e1 % e2;									// n = e1 x e2
	float a = 0.5f * std::fabs(n.length());				// area = |n| / 2
	return a;
}

// Implementation of the Intersectable interface.

/**
 * Returns true if the ray intersects the triangle.
 * This is useful for quickly determining if it's a hit or miss,
 * but no information about the hit point is returned.
 */
bool Triangle::intersect(const Ray& ray) const{

	
	Vector3D V1 = getVtxPosition(0);
	Vector3D V2 = getVtxPosition(1);
	Vector3D V3 = getVtxPosition(2);
	Vector3D O = ray.orig;
	Vector3D D = ray.dir;
	Vector3D e1, e2;
	Vector3D P, Q, T;
	float det, inv_det, u, v;
	float t;
 
	e1 = V2 - V1;
	e2 = V3 - V1;
	P = D % e2;
	det = e1 * P;
	
	if (det > -EPSILON && det < EPSILON) return false;
	
	inv_det = 1.f / det;
	T = O - V1;
	u = T * P * inv_det;
	if(u < 0.f || u > 1.f) return false;
	Q = T % e1;
	v = D * Q * inv_det;
	//The intersection lies outside of the triangle
	if(v < 0.f || u + v  > 1.f) return false;
 
	t = e2 * Q * inv_det;
 
	if(t > EPSILON) { //ray intersection
		return true;
	}

	if (t < ray.minT || t > ray.maxT) return false;
 
	// No hit, no win
	return false;
}
	
bool Triangle::intersect(const Ray& ray, Intersection& isect) const
{
	
	Vector3D V1 = getVtxPosition(0);
	Vector3D V2 = getVtxPosition(1);
	Vector3D V3 = getVtxPosition(2);
	Vector3D O = ray.orig;
	Vector3D D = ray.dir;
	Vector3D e1, e2;
	Vector3D P, Q, T;
	float det, inv_det, u, v;
	float t;

	e1 = V2 - V1;
	e2 = V3 - V1;
	P = D % e2;
	det = e1 * P;

	if (det > -EPSILON && det < EPSILON) return false;

	inv_det = 1.f / det;
	T = O - V1;
	u = T * P * inv_det;
	if (u < 0.f || u > 1.f) return false;
	Q = T % e1;
	v = D * Q * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return false;

	t = e2 * Q * inv_det;

/*
	U = w
	V = u
	W = v

*/
	if (t < ray.minT || t > ray.maxT) return false;

	if (t > EPSILON) { //ray intersection
		float w = 1 - v - u;
		float temp = u;
		u = w;
		w = v;
		v = temp;

		isect.mRay = ray;
		isect.mObject = this;						// Store ptr to the object hit by the ray (this).
		isect.mMaterial = getMaterial();
		if (!isect.mMaterial)
			isect.mMaterial = mMesh->getMaterial();		// Store ptr to the material at the hit point.
		isect.mPosition = ray.orig + t*ray.dir;		// Compute position of intersection
		isect.mNormal = u*getVtxNormal(0) + v*getVtxNormal(1) + w*getVtxNormal(2);
		isect.mNormal.normalize();
		isect.mView = -ray.dir;						// View direction is negative ray direction.
		isect.mFrontFacing = isect.mView.dot(isect.mNormal) > 0.0f;
		if (!isect.mFrontFacing) isect.mNormal = -isect.mNormal;
		isect.mTexture = u*getVtxTexture(0) + v*getVtxTexture(1) + w*getVtxTexture(2);
		isect.mHitTime = t;
		isect.mHitParam = UV(u, v);
		
		return true;
	}

	// No hit, no win
	return false;
}
	

void Triangle::getAABB(AABB& bb) const
{
	bb = AABB(getVtxPosition(0), getVtxPosition(1), getVtxPosition(2));
}

void Triangle::prepare()
{
	Vector3D n = getFaceNormal();
	
	for (int i = 0; i < 3; i++) {
		const Point3D& p0 = getVtxPosition(i);
		const Point3D& p1 = getVtxPosition((i+1)%3);
		const Point3D& p2 = getVtxPosition((i+2)%3);
		
		Vector3D e = p2 - p1;
		
		Vector3D np = n % e;
		
		float a = np.dot(p1);
		float b = np.dot(p0);
		
		float f = 1.0f / (b-a);
		float d = 1.0f - f*b;
		
		mPlanes[i] = f * np;
		mPlaneOffsets(i) = d;
	}
}

/// Returns the position of vertex i=[0,1,2].
const Point3D& Triangle::getVtxPosition(int i) const
{
	return mMesh->mVtxP[mVtx[i].p];
}

/// Returns the normal of vertex i=[0,1,2].
const Vector3D& Triangle::getVtxNormal(int i) const
{
	return mMesh->mVtxN[mVtx[i].n];
}

/// Returns the texture coordinate of vertex i=[0,1,2].
const UV& Triangle::getVtxTexture(int i) const
{
	return mMesh->mVtxUV[mVtx[i].t];
}

UV Triangle::calculateTextureDifferential(const Point3D& p, const Vector3D& dp) const
{	
	return mPlanes[0].dot(dp)*getVtxTexture(0) + mPlanes[1].dot(dp)*getVtxTexture(1) + mPlanes[2].dot(dp)*getVtxTexture(2);
}

Vector3D Triangle::calculateNormalDifferential(const Point3D& p, const Vector3D& dp, bool isFrontFacing) const
{
	Vector3D n = (mPlanes[0].dot(p)+mPlaneOffsets.x)*getVtxNormal(0) + (mPlanes[1].dot(p)+mPlaneOffsets.y)*getVtxNormal(1) + (mPlanes[2].dot(p)+mPlaneOffsets.z)*getVtxNormal(2);
	Vector3D dn = mPlanes[0].dot(dp)*getVtxNormal(0) + mPlanes[1].dot(dp)*getVtxNormal(1) + mPlanes[2].dot(dp)*getVtxNormal(2);

	float sign = isFrontFacing ? 1.0f : -1.0f;
	
	float nl = n.length();
	return sign * (n.dot(n) * dn - n.dot(dn) * n) / (nl*nl*nl);
}
