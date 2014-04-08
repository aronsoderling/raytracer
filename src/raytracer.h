/*
 *  raytracer.h
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-02-13.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#ifndef RAYTRACER_H
#define RAYTRACER_H

class Scene;
class Image;
class Camera;
class Color;

/**
 * Base class for raytracers, containinng basic functionality needed 
 * for implementing various raytracing algorithms (Whitted, Pathtracing).
 * When the function computeImage() is called, the image is raytraced by 
 * calling tracePixel() for each pixel in the output image. A very basic
 * implementation of tracePixel is provided, but to implement more advanced
 * algorithms (Whitted, Pathtracing), the tracePixel() function should be
 * overridden in a sub class.
 */
static const float nbrSamples = 9.0;
static const float samplesPerAxis = 3.0;
static const int iSamplesPerAxis = 3;
static const bool sampling = false;
static const int maxDepth = 3;
class Raytracer
{
public:
	Raytracer(Scene* scene, Image* img);
	virtual ~Raytracer() {}

	virtual void computeImage() = 0;

protected:
	Scene* mScene;		///< Ptr to the scene.
	Image* mImage;		///< Ptr to the output image.
	Camera* mCamera;	///< Ptr to the camera used for rendering.
};

#endif
