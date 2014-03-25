/*
 *  whittedtracer.cpp
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-03-10.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#include "defines.h"
#include "scene.h"
#include "camera.h"
#include "ray.h"
#include "intersection.h"
#include "material.h"
#include "whittedtracer.h"
#include "timer.h"
#include "image.h"

/**
 * Creates a Whitted raytracer. The parameters are passed on to the base class constructor.
 */
WhittedTracer::WhittedTracer(Scene* scene, Image* img) : Raytracer(scene,img)
{
}


WhittedTracer::~WhittedTracer()
{
}

/**
 * Raytraces the scene by calling tracePixel() for each pixel in the output image.
 * The function tracePixel() is responsible for computing the output color of the
 * pixel. This should be done in a sub class derived from the Raytracer base class.
 */
void WhittedTracer::computeImage()
{
	std::cout << "Raytracing..." << std::endl;
	Timer timer;
	
	Color c;
	int width = mImage->getWidth();
	int height = mImage->getHeight();
	
	// Loop over all pixels in the image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Raytrace the pixel at (x,y).
			
			c = tracePixel(x,y);
			// Store the result in the image.
			mImage->setPixel(x,y,c);
		}
		
		// Print progress approximately every 5%.
		if ((y+1) % (height/20) == 0 || (y+1) == height)
			std::cout << (100*(y+1)/height) << "% done" << std::endl;
	}
	
	std::cout << "Done in " << timer.stop() << " seconds" << std::endl;
}

/**
 * Compute the color of the pixel at (x,y) by raytracing. 
 * The default implementation here just traces through the center of
 * the pixel.
 */
Color WhittedTracer::tracePixel(int x, int y)
{
	Color pixelColor = Color(0.0f, 0.0f, 0.0f);

	
	for(int i = 0; i <3; ++i){
		for(int j = 0; j <3; ++j){
			float cx = (float)x + j / 3.0f + uniform()/9.0f; 
			float cy = (float)y + i / 3.0f + uniform()/9.0f;
			Ray ray = mCamera->getRay(cx,cy);
			pixelColor += trace(ray, 0); 
		}
	}
	return pixelColor / 9.0f;
}

/**
 * Computes the radiance returned by tracing the ray r.
 */
Color WhittedTracer::trace(const Ray& ray, int depth)
{
	Color colorOut = Color(0,0,0);
	if(depth < 3){
		Intersection is;
		if (mScene->intersect(ray, is)){
			int n = 0;
			n = mScene->getNumberOfLights();
			Vector3D lightVec;
			PointLight* l;
			for(int i = 0; i != n; ++i){
				Color radiance = Color(0,0,0);
				l = mScene->getLight(i);
				if(!mScene->intersect(is.getShadowRay(l))){
					lightVec = l->getWorldPosition();
					lightVec -= is.mPosition;
					lightVec.normalize();
					radiance = l->getRadiance();
					colorOut += radiance * is.mMaterial->evalBRDF(is, lightVec) * max(lightVec * is.mNormal, 0.0f);
				}
			}
			if(is.mMaterial->getReflectivity(is) > 0){
				colorOut += is.mMaterial->getReflectivity(is) * trace(is.getReflectedRay(), depth+1);
			}
			if(is.mMaterial->getTransparency(is) > 0){
				colorOut += is.mMaterial->getTransparency(is) *trace(is.getRefractedRay(), depth+1);
			}
		}
	}
	return colorOut;
	
}

