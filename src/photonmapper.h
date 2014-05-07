/*
 *  PhotonMapper.h
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-03-10.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#ifndef PHOTONMAPPER_H
#define PHOTONMAPPER_H

#include "raytracer.h"
#include "bvhaccelerator.h"

/**
 * Class implementing a simple Whitted-style raytracer. 
 * The tracePixel() function is called once for each pixel on the
 * screen, and is responsible for computing the output color by
 * tracing a ray through the scene. At each intersection, the
 * contributions from direct illumination, reflection and refraction
 * should be evaluated and summed up.
 */

struct Hitpoint {
	Intersection is;
	int pixelX, pixelY;
	float pixelWeight;
	float radius;
	Color directIllumination;
	float photonCount = 0;
	int newPhotonCount = 0;
	Color totalFlux = Color(0.0f,0.0f,0.0f);
};

class PhotonMapper : public Raytracer
{
public:
	PhotonMapper(Scene* scene, Image* img);
	~PhotonMapper();

	virtual void computeImage();
	
protected:
	Color tracePixel(int x, int y);
	Color trace(const Ray& ray, int depth);
	void firstPass(int x, int y);
	
	BVHAccelerator hitpointBVH;
};

#endif
