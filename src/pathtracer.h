/*
 *  Pathtracer.h
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-03-10.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "raytracer.h"

/**
 * Class implementing a simple Whitted-style raytracer. 
 * The tracePixel() function is called once for each pixel on the
 * screen, and is responsible for computing the output color by
 * tracing a ray through the scene. At each intersection, the
 * contributions from direct illumination, reflection and refraction
 * should be evaluated and summed up.
 */

class PathTracer : public Raytracer
{
public:
	PathTracer(Scene* scene, Image* img);
	~PathTracer();

	virtual void computeImage();
	
protected:
	Color tracePixel(int x, int y);
	Color trace(const Ray& ray, int depth);
};

#endif
