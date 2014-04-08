/*
 *  Pathtracer.cpp
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
#include "pathtracer.h"
#include "timer.h"
#include "image.h"

/**
 * Creates a Path raytracer. The parameters are passed on to the base class constructor.
 */
PathTracer::PathTracer(Scene* scene, Image* img) : Raytracer(scene,img)
{
}


PathTracer::~PathTracer()
{
}

/**
 * Raytraces the scene by calling tracePixel() for each pixel in the output image.
 * The function tracePixel() is responsible for computing the output color of the
 * pixel. This should be done in a sub class derived from the Raytracer base class.
 */
void PathTracer::computeImage()
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
		if ((y + 1) % (height / 20) == 0 || (y + 1) == height){
			std::cout << (100 * (y + 1) / height) << "% done" << std::endl;
			//std::cout << "Rays/second: " << nbrRays / timer.stop() << std::endl;
			//std::cout << "Rays: " << nbrRays << std::endl;
			//std::cout << "Time: " << timer.stop() << std::endl;
		}
	}
	//std::cout << "Total number of rays: " << nbrRays << std::endl;
	std::cout << "Done in: " << timer.stop() << " seconds" << std::endl;
}

/**
 * Compute the color of the pixel at (x,y) by raytracing. 
 * The default implementation here just traces through the center of
 * the pixel.
 */
Color PathTracer::tracePixel(int x, int y)
{
	Color pixelColor = Color(0.0f, 0.0f, 0.0f);

	//super sampling, samples / pixel
	if (sampling){
		for (int i = 0; i < iSamplesPerAxis; ++i){
			for (int j = 0; j < iSamplesPerAxis; ++j){
				float cx = (float)x + j / samplesPerAxis + uniform() / samplesPerAxis;
				float cy = (float)y + i / samplesPerAxis + uniform() / samplesPerAxis;
				Ray ray = mCamera->getRay(cx, cy);
				pixelColor += trace(ray, 0);
			}
		}
		pixelColor /= nbrSamples;
	}
	else{
		float cx = (float)x + 0.5f;
		float cy = (float)y + 0.5f;
		Ray ray = mCamera->getRay(cx, cy);
		pixelColor = trace(ray, 0);
	}
	return pixelColor;
}

/**
 * Computes the radiance returned by tracing the ray r.
 */
Color PathTracer::trace(const Ray& ray, int depth)
{
	Color colorOut = Color(0,0,0);
	Intersection is;
	++nbrRays;
	if (mScene->intersect(ray, is)){
		Color reflectedC, refractedC, emittedC;
		Material* m = is.mMaterial;
		float reflectivity = m->getReflectivity(is);
		float transparency = m->getTransparency(is);
		if (depth < maxDepth){
			reflectedC += trace(is.getReflectedRay(), depth + 1);
			refractedC += trace(is.getRefractedRay(), depth + 1);
		}
		for (int i = 0; i < mScene->getNumberOfLights(); ++i){
			PointLight* l = mScene->getLight(i);
			if(!mScene->intersect(is.getShadowRay(l))){
				Vector3D lightVec = l->getWorldPosition() - is.mPosition;
				float d2 = lightVec.length2();
				lightVec.normalize();
				Color radiance = l->getRadiance();
				Color brdf = is.mMaterial->evalBRDF(is, lightVec);
				float angle = max(lightVec * is.mNormal, 0.0f);
				emittedC += radiance * brdf * angle / d2;
			}
		}
		float x, y, z;
		Vector3D v(1.f,1.f,1.f);
		while (v.length2 > 1){
			 v = Vector3D(uniform() * 2 - 1, uniform() * 2 - 1, uniform() * 2 - 1);
		}

		v.normalize();
		if (sin(v * is.mNormal) > 0.f )
			v = v - 2 * (v * is.mNormal) * is.mNormal;

		if (depth < maxDepth){
			Ray rayCopy(is.mPosition, v, 0.001f);
			trace(rayCopy, depth + 1);
		}
		colorOut = emittedC * (1 - transparency - reflectivity) + refractedC * transparency + reflectedC * reflectivity;
		//colorOut *= (1 - is.mMaterial->getReflectivity(is) - is.mMaterial->getTransparency(is));			
	}
return colorOut;
	
}

